module phantomdb-gorm

go 1.19

require (
	gorm.io/gorm v1.25.0
	gorm.io/driver/mysql v1.5.0
)

require (
	github.com/jinzhu/inflection v1.0.0 // indirect
	github.com/jinzhu/now v1.1.5 // indirect
	github.com/mattn/go-sqlite3 v1.14.16 // indirect
	golang.org/x/crypto v0.0.0-20220622213112-05595931fe9d // indirect
	golang.org/x/text v0.4.0 // indirect
)

replace phantomdb-go => ../clients/go