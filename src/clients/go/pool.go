// Package pool provides connection pooling functionality for PhantomDB clients.
package pool

import (
	"sync"
	"time"
	
	"phantomdb-go" // This would be the actual import path
)

// Config represents the configuration for a connection pool.
type Config struct {
	BaseURL         string
	MinConnections  int
	MaxConnections  int
	AcquireTimeout  time.Duration
	IdleTimeout     time.Duration
}

// PhantomDBPool represents a connection pool for PhantomDB clients.
type PhantomDBPool struct {
	config  Config
	pool    chan *phantomdb.PhantomDB
	used    map[*phantomdb.PhantomDB]bool
	mutex   sync.Mutex
	closed  bool
}

// New creates a new PhantomDB connection pool.
func New(config Config) *PhantomDBPool {
	if config.MinConnections <= 0 {
		config.MinConnections = 2
	}
	if config.MaxConnections <= 0 {
		config.MaxConnections = 10
	}
	if config.AcquireTimeout <= 0 {
		config.AcquireTimeout = 30 * time.Second
	}
	if config.IdleTimeout <= 0 {
		config.IdleTimeout = 60 * time.Second
	}
	if config.BaseURL == "" {
		config.BaseURL = "http://localhost:8080"
	}
	
	pool := &PhantomDBPool{
		config: config,
		pool:   make(chan *phantomdb.PhantomDB, config.MaxConnections),
		used:   make(map[*phantomdb.PhantomDB]bool),
	}
	
	// Initialize the pool
	pool.initializePool()
	
	return pool
}

// initializePool initializes the connection pool with minimum connections.
func (p *PhantomDBPool) initializePool() {
	for i := 0; i < p.config.MinConnections; i++ {
		p.createConnection()
	}
}

// createConnection creates a new connection and adds it to the pool.
func (p *PhantomDBPool) createConnection() {
	if len(p.used)+len(p.pool) >= p.config.MaxConnections {
		return
	}
	
	connection := phantomdb.New(p.config.BaseURL)
	select {
	case p.pool <- connection:
	default:
		// Pool is full, discard the connection
	}
}

// Acquire acquires a connection from the pool.
func (p *PhantomDBPool) Acquire() (*phantomdb.PhantomDB, error) {
	if p.closed {
		return nil, &phantomdb.PhantomDBError{Message: "Connection pool is closed"}
	}
	
	timeout := time.After(p.config.AcquireTimeout)
	
	for {
		select {
		case connection := <-p.pool:
			// Check if connection is still valid
			// In a real implementation, we might want to ping the server
			p.mutex.Lock()
			p.used[connection] = true
			p.mutex.Unlock()
			return connection, nil
		case <-timeout:
			return nil, &phantomdb.PhantomDBError{Message: "Timeout acquiring connection from pool"}
		default:
			// No available connections, check if we can create a new one
			p.mutex.Lock()
			if len(p.used) < p.config.MaxConnections {
				p.mutex.Unlock()
				p.createConnection()
				continue
			}
			p.mutex.Unlock()
			
			// Wait a bit before trying again
			time.Sleep(100 * time.Millisecond)
		}
	}
}

// Release releases a connection back to the pool.
func (p *PhantomDBPool) Release(connection *phantomdb.PhantomDB) {
	if p.closed {
		return
	}
	
	p.mutex.Lock()
	defer p.mutex.Unlock()
	
	if p.used[connection] {
		delete(p.used, connection)
		select {
		case p.pool <- connection:
		default:
			// Pool is full, discard the connection
		}
	}
}

// Close closes the connection pool and all connections.
func (p *PhantomDBPool) Close() {
	p.mutex.Lock()
	defer p.mutex.Unlock()
	
	p.closed = true
	close(p.pool)
	
	// Clear the used connections
	p.used = make(map[*phantomdb.PhantomDB]bool)
}

// Status returns the current pool status.
func (p *PhantomDBPool) Status() map[string]int {
	p.mutex.Lock()
	defer p.mutex.Unlock()
	
	return map[string]int{
		"available": len(p.pool),
		"used":      len(p.used),
		"min":       p.config.MinConnections,
		"max":       p.config.MaxConnections,
	}
}