# PhantomDB Diesel Integration

This package provides a Diesel ORM integration that allows Rust applications to use PhantomDB as their database backend through its REST API.

## Overview

The PhantomDB Diesel integration enables Rust developers to leverage PhantomDB's advanced features while maintaining the familiar Diesel ORM interface. This integration translates Diesel operations into PhantomDB REST API calls.

## Features

- Full Diesel compatibility
- Automatic schema translation
- Query translation from Diesel to PhantomDB
- Transaction support
- Migration support

## Installation

Add this to your `Cargo.toml`:

```toml
[dependencies]
phantomdb-diesel = "1.0"
diesel = { version = "2.0", features = ["postgres"] }
```

## Configuration

To use PhantomDB with Diesel, configure your database connection:

```rust
use phantomdb_diesel::PhantomDBConnection;
use diesel::prelude::*;

// Establish connection with PhantomDB
let connection = PhantomDBConnection::establish("http://localhost:8080")?;
```

## Usage

Once configured, you can use Diesel as usual:

```rust
use diesel::prelude::*;
use phantomdb_diesel::PhantomDBConnection;

#[derive(Queryable, Selectable)]
#[diesel(table_name = crate::schema::users)]
#[diesel(check_for_backend(diesel::pg::Pg))]
pub struct User {
    pub id: i32,
    pub first_name: String,
    pub last_name: String,
    pub email: String,
    pub age: i32,
    pub active: bool,
}

#[derive(Insertable)]
#[diesel(table_name = crate::schema::users)]
pub struct NewUser {
    pub first_name: String,
    pub last_name: String,
    pub email: String,
    pub age: i32,
    pub active: bool,
}

// Create
let new_user = NewUser {
    first_name: "John".to_string(),
    last_name: "Doe".to_string(),
    email: "john.doe@example.com".to_string(),
    age: 30,
    active: true,
};

diesel::insert_into(users::table)
    .values(&new_user)
    .execute(&mut connection)?;

// Read
let users = users::table
    .filter(users::age.gt(18))
    .load::<User>(&mut connection)?;

let user = users::table
    .find(1)
    .first::<User>(&mut connection)?;

// Update
diesel::update(users::table.filter(users::id.eq(1)))
    .set(users::age.eq(35))
    .execute(&mut connection)?;

// Delete
diesel::delete(users::table.filter(users::id.eq(1)))
    .execute(&mut connection)?;
```

## Data Type Mapping

The integration automatically maps Rust field types to PhantomDB types:

| Rust Type | PhantomDB Type |
|-----------|----------------|
| i8, i16, i32 | INTEGER |
| i64 | BIGINT |
| f32 | FLOAT |
| f64 | DOUBLE |
| bool | BOOLEAN |
| String | VARCHAR |
| chrono::NaiveDateTime | TIMESTAMP |
| Vec<u8> | BLOB |

## Advanced Features

### Transactions

```rust
use diesel::Connection;

connection.transaction::<_, diesel::result::Error, _>(|conn| {
    diesel::insert_into(users::table)
        .values(&new_user)
        .execute(conn)?;
        
    diesel::insert_into(profiles::table)
        .values(&new_profile)
        .execute(conn)?;
        
    Ok(())
})?;
```

### Complex Queries

```rust
use diesel::prelude::*;

// Complex filtering
let users = users::table
    .filter(users::age.between(18, 65))
    .filter(users::email.like("%@example.com"))
    .filter(users::active.eq(true))
    .order(users::created_at.desc())
    .limit(10)
    .load::<User>(&mut connection)?;

// Joins
let users_with_profiles = users::table
    .inner_join(profiles::table)
    .load::<(User, Profile)>(&mut connection)?;
```

## Requirements

- Rust >= 1.60
- Diesel >= 2.0
- phantomdb-rust >= 1.0

## Development

To run tests:

```bash
cargo test
```

## License

MIT License

## Contributing

Contributions are welcome! Please read our contributing guidelines before submitting pull requests.