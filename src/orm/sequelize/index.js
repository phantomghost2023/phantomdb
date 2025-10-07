/**
 * PhantomDB Sequelize Dialect
 * 
 * This module provides a Sequelize dialect for PhantomDB.
 */

const { Sequelize, DataTypes, Model } = require('sequelize');
const { PhantomDB } = require('phantomdb-js');

/**
 * PhantomDB Dialect for Sequelize
 */
class PhantomDBDialect {
  /**
   * Constructor for the PhantomDB dialect
   * @param {string} connectionString - Connection string for PhantomDB
   */
  constructor(connectionString) {
    this.connectionString = connectionString || 'http://localhost:8080';
    this.db = new PhantomDB(this.connectionString);
    this.models = new Map(); // Store model definitions
    this.associations = new Map(); // Store associations
  }

  /**
   * Initialize the dialect
   * @param {Sequelize} sequelize - Sequelize instance
   */
  async init(sequelize) {
    // Check connection
    try {
      await this.db.healthCheck();
      console.log('Connected to PhantomDB successfully');
    } catch (error) {
      throw new Error(`Failed to connect to PhantomDB: ${error.message}`);
    }
  }

  /**
   * Register a model with the dialect
   * @param {string} modelName - Name of the model
   * @param {Object} modelDefinition - Model definition
   */
  registerModel(modelName, modelDefinition) {
    this.models.set(modelName, modelDefinition);
  }

  /**
   * Define an association between two models
   * @param {string} sourceModel - Source model name
   * @param {string} targetModel - Target model name
   * @param {string} associationType - Type of association (hasOne, hasMany, belongsTo, belongsToMany)
   * @param {Object} options - Association options
   */
  defineAssociation(sourceModel, targetModel, associationType, options = {}) {
    const associationKey = `${sourceModel}-${targetModel}`;
    this.associations.set(associationKey, {
      source: sourceModel,
      target: targetModel,
      type: associationType,
      options
    });
  }

  /**
   * Create a database
   * @param {string} databaseName - Name of the database to create
   */
  async createDatabase(databaseName) {
    try {
      await this.db.createDatabase(databaseName);
    } catch (error) {
      throw new Error(`Failed to create database ${databaseName}: ${error.message}`);
    }
  }

  /**
   * Drop a database
   * @param {string} databaseName - Name of the database to drop
   */
  async dropDatabase(databaseName) {
    try {
      await this.db.dropDatabase(databaseName);
    } catch (error) {
      throw new Error(`Failed to drop database ${databaseName}: ${error.message}`);
    }
  }

  /**
   * Create a table
   * @param {string} databaseName - Name of the database
   * @param {string} tableName - Name of the table to create
   * @param {Object} attributes - Table attributes definition
   */
  async createTable(databaseName, tableName, attributes) {
    try {
      // Convert Sequelize attributes to PhantomDB column format
      const columns = Object.keys(attributes).map(attrName => {
        const attr = attributes[attrName];
        return {
          name: attrName,
          type: this._mapDataType(attr.type)
        };
      });

      // Add primary key if not exists
      if (!columns.some(col => col.name === 'id')) {
        columns.unshift({
          name: 'id',
          type: 'integer'
        });
      }

      await this.db.createTable(databaseName, tableName, columns);
    } catch (error) {
      throw new Error(`Failed to create table ${tableName}: ${error.message}`);
    }
  }

  /**
   * Drop a table
   * @param {string} databaseName - Name of the database
   * @param {string} tableName - Name of the table to drop
   */
  async dropTable(databaseName, tableName) {
    try {
      await this.db.dropTable(databaseName, tableName);
    } catch (error) {
      throw new Error(`Failed to drop table ${tableName}: ${error.message}`);
    }
  }

  /**
   * Insert data into a table
   * @param {string} databaseName - Name of the database
   * @param {string} tableName - Name of the table
   * @param {Object} values - Data to insert
   */
  async insert(databaseName, tableName, values) {
    try {
      const result = await this.db.insert(databaseName, tableName, values);
      return result;
    } catch (error) {
      throw new Error(`Failed to insert data into ${tableName}: ${error.message}`);
    }
  }

  /**
   * Select data from a table
   * @param {string} databaseName - Name of the database
   * @param {string} tableName - Name of the table
   * @param {Object} options - Query options
   */
  async select(databaseName, tableName, options = {}) {
    try {
      let condition = null;
      let orderBy = null;
      let limitOffset = null;
      
      // Build condition from options
      if (options.where) {
        condition = this._buildWhereClause(options.where);
      }
      
      // Build ORDER BY clause
      if (options.order) {
        orderBy = this._buildOrderByClause(options.order);
      }
      
      // Build LIMIT/OFFSET clause
      if (options.limit || options.offset) {
        limitOffset = this._buildLimitOffsetClause(options);
      }
      
      // For now, we'll build a more complex query string
      // In a full implementation, this would be more sophisticated
      let queryParts = [];
      
      if (condition) {
        queryParts.push(`WHERE ${condition}`);
      }
      
      if (orderBy) {
        queryParts.push(`ORDER BY ${orderBy}`);
      }
      
      if (limitOffset) {
        queryParts.push(limitOffset);
      }
      
      // For demonstration, we'll just pass the condition for now
      // A full implementation would need to handle ORDER BY and LIMIT/OFFSET
      const result = await this.db.select(databaseName, tableName, condition);
      return result;
    } catch (error) {
      throw new Error(`Failed to select data from ${tableName}: ${error.message}`);
    }
  }

  /**
   * Update data in a table
   * @param {string} databaseName - Name of the database
   * @param {string} tableName - Name of the table
   * @param {Object} values - Data to update
   * @param {Object} options - Update options
   */
  async update(databaseName, tableName, values, options = {}) {
    try {
      let condition = null;
      
      // Build condition from options
      if (options.where) {
        condition = this._buildWhereClause(options.where);
      }
      
      const result = await this.db.update(databaseName, tableName, values, condition);
      return result;
    } catch (error) {
      throw new Error(`Failed to update data in ${tableName}: ${error.message}`);
    }
  }

  /**
   * Delete data from a table
   * @param {string} databaseName - Name of the database
   * @param {string} tableName - Name of the table
   * @param {Object} options - Delete options
   */
  async delete(databaseName, tableName, options = {}) {
    try {
      let condition = null;
      
      // Build condition from options
      if (options.where) {
        condition = this._buildWhereClause(options.where);
      }
      
      const result = await this.db.delete(databaseName, tableName, condition);
      return result;
    } catch (error) {
      throw new Error(`Failed to delete data from ${tableName}: ${error.message}`);
    }
  }

  /**
   * Map Sequelize data types to PhantomDB types
   * @param {Object} sequelizeType - Sequelize data type
   * @returns {string} PhantomDB type
   */
  _mapDataType(sequelizeType) {
    if (sequelizeType === DataTypes.INTEGER || sequelizeType === DataTypes.BIGINT) {
      return 'integer';
    } else if (sequelizeType === DataTypes.STRING || sequelizeType === DataTypes.TEXT) {
      return 'string';
    } else if (sequelizeType === DataTypes.BOOLEAN) {
      return 'boolean';
    } else if (sequelizeType === DataTypes.DATE) {
      return 'timestamp';
    } else if (sequelizeType === DataTypes.FLOAT || sequelizeType === DataTypes.DOUBLE) {
      return 'float';
    } else {
      return 'string'; // Default to string
    }
  }

  /**
   * Build WHERE clause from Sequelize where object
   * @param {Object} where - Sequelize where object
   * @returns {string} WHERE clause string
   */
  _buildWhereClause(where) {
    if (!where) return null;
    
    const conditions = [];
    for (const [key, value] of Object.entries(where)) {
      // Handle different operators
      if (typeof value === 'object' && value !== null) {
        for (const [operator, opValue] of Object.entries(value)) {
          switch (operator) {
            case Sequelize.Op.eq:
              if (typeof opValue === 'string') {
                conditions.push(`${key} = '${opValue}'`);
              } else {
                conditions.push(`${key} = ${opValue}`);
              }
              break;
            case Sequelize.Op.ne:
              if (typeof opValue === 'string') {
                conditions.push(`${key} != '${opValue}'`);
              } else {
                conditions.push(`${key} != ${opValue}`);
              }
              break;
            case Sequelize.Op.gt:
              conditions.push(`${key} > ${opValue}`);
              break;
            case Sequelize.Op.gte:
              conditions.push(`${key} >= ${opValue}`);
              break;
            case Sequelize.Op.lt:
              conditions.push(`${key} < ${opValue}`);
              break;
            case Sequelize.Op.lte:
              conditions.push(`${key} <= ${opValue}`);
              break;
            case Sequelize.Op.like:
              conditions.push(`${key} LIKE '${opValue}'`);
              break;
            case Sequelize.Op.in:
              const inValues = opValue.map(v => typeof v === 'string' ? `'${v}'` : v).join(', ');
              conditions.push(`${key} IN (${inValues})`);
              break;
            default:
              if (typeof opValue === 'string') {
                conditions.push(`${key} = '${opValue}'`);
              } else {
                conditions.push(`${key} = ${opValue}`);
              }
          }
        }
      } else {
        if (typeof value === 'string') {
          conditions.push(`${key} = '${value}'`);
        } else {
          conditions.push(`${key} = ${value}`);
        }
      }
    }
    
    return conditions.join(' AND ');
  }

  /**
   * Build ORDER BY clause
   * @param {Array} order - Order array
   * @returns {string} ORDER BY clause string
   */
  _buildOrderByClause(order) {
    if (!order || order.length === 0) return null;
    
    const orderParts = order.map(([field, direction]) => {
      return `${field} ${direction.toUpperCase()}`;
    });
    
    return orderParts.join(', ');
  }

  /**
   * Build LIMIT and OFFSET clause
   * @param {Object} options - Query options
   * @returns {string} LIMIT/OFFSET clause string
   */
  _buildLimitOffsetClause(options) {
    const clauses = [];
    
    if (options.limit) {
      clauses.push(`LIMIT ${options.limit}`);
    }
    
    if (options.offset) {
      clauses.push(`OFFSET ${options.offset}`);
    }
    
    return clauses.join(' ');
  }

  /**
   * Execute a raw query
   * @param {string} databaseName - Name of the database
   * @param {string} query - SQL query to execute
   * @returns {Promise} Query result
   */
  async query(databaseName, query) {
    try {
      const result = await this.db.executeQuery(databaseName, query);
      return result;
    } catch (error) {
      throw new Error(`Failed to execute query: ${error.message}`);
    }
  }

  /**
   * Begin a transaction
   * @returns {Promise<string>} Transaction ID
   */
  async beginTransaction() {
    try {
      const transactionId = await this.db.beginTransaction();
      return transactionId;
    } catch (error) {
      throw new Error(`Failed to begin transaction: ${error.message}`);
    }
  }

  /**
   * Commit a transaction
   * @param {string} transactionId - Transaction ID
   * @returns {Promise} Commit result
   */
  async commitTransaction(transactionId) {
    try {
      const result = await this.db.commitTransaction(transactionId);
      return result;
    } catch (error) {
      throw new Error(`Failed to commit transaction: ${error.message}`);
    }
  }

  /**
   * Rollback a transaction
   * @param {string} transactionId - Transaction ID
   * @returns {Promise} Rollback result
   */
  async rollbackTransaction(transactionId) {
    try {
      const result = await this.db.rollbackTransaction(transactionId);
      return result;
    } catch (error) {
      throw new Error(`Failed to rollback transaction: ${error.message}`);
    }
  }
}

// Export the dialect
module.exports = { PhantomDBDialect };