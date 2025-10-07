/**
 * PhantomDB JavaScript Client
 * 
 * Main entry point for the PhantomDB JavaScript client module.
 */

const { PhantomDB, PhantomDBError, connect } = require('./phantomdb.js');

module.exports = {
  PhantomDB,
  PhantomDBError,
  connect
};