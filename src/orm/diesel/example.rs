//! Example usage of PhantomDB Diesel integration
//!
//! This example demonstrates how to use the PhantomDB Diesel integration.

/// Main function to demonstrate usage
fn main() {
    println!("PhantomDB Diesel Integration Example");
    println!("==================================");
    println!("\nThis example shows how the integration would work with Diesel.");
    
    println!("\nConfiguration:");
    println!("```rust");
    println!("use phantomdb_diesel::PhantomDBConnection;");
    println!("use diesel::prelude::*;");
    println!("");
    println!("// Establish connection with PhantomDB");
    println!("let connection = PhantomDBConnection::establish(\"http://localhost:8080\")?;");
    println!("```");
    
    println!("\nModel definition:");
    println!("```rust");
    println!("#[derive(Queryable, Selectable)]");
    println!("#[diesel(table_name = crate::schema::users)]");
    println!("#[diesel(check_for_backend(diesel::pg::Pg))]");
    println!("pub struct User {{");
    println!("    pub id: i32,");
    println!("    pub first_name: String,");
    println!("    pub last_name: String,");
    println!("    pub email: String,");
    println!("    pub age: i32,");
    println!("    pub active: bool,");
    println!("}}");
    println!("");
    println!("#[derive(Insertable)]");
    println!("#[diesel(table_name = crate::schema::users)]");
    println!("pub struct NewUser {{");
    println!("    pub first_name: String,");
    println!("    pub last_name: String,");
    println!("    pub email: String,");
    println!("    pub age: i32,");
    println!("    pub active: bool,");
    println!("}}");
    println!("```");
    
    println!("\nUsage examples:");
    println!("```rust");
    println!("// Create");
    println!("let new_user = NewUser {{");
    println!("    first_name: \"John\".to_string(),");
    println!("    last_name: \"Doe\".to_string(),");
    println!("    email: \"john.doe@example.com\".to_string(),");
    println!("    age: 30,");
    println!("    active: true,");
    println!("}};");
    println!("");
    println!("diesel::insert_into(users::table)");
    println!("    .values(&new_user)");
    println!("    .execute(&mut connection)?;");
    println!("");
    println!("// Read");
    println!("let users = users::table");
    println!("    .filter(users::age.gt(18))");
    println!("    .load::<User>(&mut connection)?;");
    println!("");
    println!("let user = users::table");
    println!("    .find(1)");
    println!("    .first::<User>(&mut connection)?;");
    println!("");
    println!("// Update");
    println!("diesel::update(users::table.filter(users::id.eq(1)))");
    println!("    .set(users::age.eq(35))");
    println!("    .execute(&mut connection)?;");
    println!("");
    println!("// Delete");
    println!("diesel::delete(users::table.filter(users::id.eq(1)))");
    println!("    .execute(&mut connection)?;");
    println!("```");
    
    println!("\nExample completed successfully!");
    println!("\nNote: This is a conceptual example. To run it, you would need to:");
    println!("1. Install Diesel and phantomdb-rust");
    println!("2. Configure the PhantomDB Diesel integration");
    println!("3. Run the example with a PhantomDB server running");
}