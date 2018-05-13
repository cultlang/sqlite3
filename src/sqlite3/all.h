#pragma once
#include "sqlite3/common.h"

namespace cultlang {
namespace sqlite3 {
	extern craft::lisp::BuiltinModuleDescription BuiltinSqlite3;

	CULTLANG_SQLITE3_EXPORTED craft::instance<craft::lisp::Module> make_sqlite3_bindings(craft::instance<craft::lisp::Namespace> ns, craft::instance<> loader);

	class SqliteDatabase;

	class SqliteStatement
		: public virtual craft::types::Object
	{
	public:
		CULTLANG_SQLITE3_EXPORTED CRAFT_OBJECT_DECLARE(cultlang::sqlite3::SqliteStatement);

		::sqlite3_stmt* data;
		
		friend class SqliteDatabase;

	public:
		craft::types::instance<> step();
	public:
		
	};


	class SqliteDatabase
		: public virtual craft::types::Object
	{
		CULTLANG_SQLITE3_EXPORTED CRAFT_OBJECT_DECLARE(cultlang::sqlite3::SqliteDatabase);

		::sqlite3* data;
		std::string _path;
	public:
		inline SqliteDatabase(std::string dbname)
			: _path(dbname)
			, data(nullptr)
		{

			int res = sqlite3_open(dbname.c_str(), &data);
			if (res != SQLITE_OK)
			{
				throw stdext::exception(sqlite3_errmsg(data));
			}
		}

		inline SqliteDatabase(SqliteDatabase&& other) noexcept
		{
			if (other.data)
			{
				data = std::move(other.data);
				other.data = nullptr;
			}
		}

		inline ~SqliteDatabase()
		{
			if (data) sqlite3_close(data);
		}

		craft::instance<SqliteStatement> makeCommand(std::string query, craft::types::VarArgs<craft::instance<>> params);
		
	};

	
}}
