//! Type definitions for the PhantomDB Rust client.

use serde::{Deserialize, Serialize};

/// Represents a table column definition.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Column {
    /// Column name
    pub name: String,
    /// Column type
    #[serde(rename = "type")]
    pub type_: String,
}

/// Database information.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DatabaseInfo {
    /// Database name
    pub name: String,
}

/// Table information.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TableInfo {
    /// Table name
    pub name: String,
}