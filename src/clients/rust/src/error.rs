//! Error types for the PhantomDB Rust client.

use thiserror::Error;

/// Custom error type for PhantomDB client operations.
#[derive(Error, Debug)]
pub enum PhantomDBError {
    /// HTTP request error
    #[error("HTTP error: {0}")]
    HttpError(reqwest::StatusCode, String),

    /// JSON serialization/deserialization error
    #[error("JSON error: {0}")]
    JsonError(#[from] serde_json::Error),

    /// HTTP communication error
    #[error("Request error: {0}")]
    RequestError(#[from] reqwest::Error),

    /// URL parsing error
    #[error("URL error: {0}")]
    UrlError(#[from] url::ParseError),

    /// Custom error message
    #[error("PhantomDB error: {0}")]
    Custom(String),
}

impl PhantomDBError {
    /// Create a custom PhantomDB error.
    pub fn custom(message: impl Into<String>) -> Self {
        PhantomDBError::Custom(message.into())
    }
}