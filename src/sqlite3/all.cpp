#include "sqlite3/common.h"

#include "all.h"

#include "lisp/library/system/prelude.h"
#include "lisp/semantics/cult/calling.h"
#include "types/cpp/cpp_interface.h"

using namespace craft;
using namespace craft::lisp;
using namespace craft::types;
using namespace cultlang::sqlite3;

#define lMM semantics->builtin_implementMultiMethod
#define sQL "sqlite"


#include "typedefs.h"


CRAFT_DEFINE(cultlang::sqlite3::SqliteDatabase)
{
	_.defaults();
}

CRAFT_DEFINE(cultlang::sqlite3::SqliteStatement)
{
	_.defaults();
}

instance<SqliteStatement> SqliteDatabase::makeCommand(std::string query, VarArgs<instance<>> params)
{
	auto res = instance<SqliteStatement>::make();
	const  char* f;
	auto err = sqlite3_prepare(data, query.c_str(), query.size(), &res->data, &f);
	if (err != SQLITE_OK)
	{
		throw stdext::exception(sqlite3_errmsg(data));
	}

	auto ptr = res->data;
	auto fail = true;
	for (auto i = 0; i < params.args.size(); i++)
	{
		auto ar = params.args[i];
		
		auto tname = ar.typeId().toString();
		if (!ar && sqlite3_bind_null(ptr, i) != SQLITE_OK) throw stdext::exception(sqlite3_errmsg(data));

		else  if (ar.isType<std::string>())
		{
			auto s = ar.asType<std::string>();;
			if(sqlite3_bind_text(ptr, i + 1, s->c_str(), s->size(), 0) != SQLITE_OK) throw stdext::exception(sqlite3_errmsg(data));
		}

		else if (ar.isType<uint8_t>())
		{
			if(sqlite3_bind_int(ptr, i + 1, *ar.asType<uint8_t>()) != SQLITE_OK) throw stdext::exception(sqlite3_errmsg(data));
		}
		else if (ar.isType<uint16_t>())
		{
			if(sqlite3_bind_int(ptr, i + 1, *ar.asType<uint16_t>()) != SQLITE_OK) throw stdext::exception(sqlite3_errmsg(data));
		}
		else if (ar.isType<uint32_t>())
		{
			if(sqlite3_bind_int(ptr, i + 1, *ar.asType<uint32_t>()) != SQLITE_OK) throw stdext::exception(sqlite3_errmsg(data));
		}
		if (ar.isType<uint64_t>())
		{
			if (sqlite3_bind_int64(ptr, i + 1, *ar.asType<uint64_t>()) != SQLITE_OK)throw stdext::exception(sqlite3_errmsg(data));
		}
		else if (ar.isType<int8_t>())
		{
			if(sqlite3_bind_int(ptr, i + 1, *ar.asType<int8_t>()) != SQLITE_OK)throw stdext::exception(sqlite3_errmsg(data));
		}
		else if (ar.isType<int16_t>())
		{
			if(sqlite3_bind_int(ptr, i + 1, *ar.asType<int16_t>()) != SQLITE_OK) throw stdext::exception(sqlite3_errmsg(data));
		}
		if (ar.isType<int32_t>())
		{
			if(sqlite3_bind_int(ptr, i + 1, *ar.asType<int32_t>()) != SQLITE_OK) throw stdext::exception(sqlite3_errmsg(data));
		}
		else if (ar.isType<int64_t>())
		{
			if(sqlite3_bind_int64(ptr, i + 1, *ar.asType<int64_t>()) != SQLITE_OK) throw stdext::exception(sqlite3_errmsg(data));
		}

		else if (ar.isType<float>())
		{
			if(sqlite3_bind_double(ptr, i + 1, *ar.asType<float>()) != SQLITE_OK)throw stdext::exception(sqlite3_errmsg(data));
		}
		else if (ar.isType<double>())
		{
			if(sqlite3_bind_double(ptr, i + 1, *ar.asType<double>()) != SQLITE_OK) throw stdext::exception(sqlite3_errmsg(data));
		}

		else if (ar.hasFeature<PStringer>())
		{
			auto s = ar.getFeature<PStringer>()->toString(ar);
			auto err = sqlite3_bind_text(ptr, i + 1, s.c_str(), s.size(), 0);
			if (err != SQLITE_OK) throw stdext::exception(sqlite3_errmsg(data));
		}
			
	}

	return res;
}

instance<> SqliteStatement::step()
{
	auto ret = sqlite3_step(data);
	auto ptr = data;
	switch (ret)
	{
	case SQLITE_BUSY:
		throw stdext::exception("Unable to Aquire Sqlite Lock");
	case SQLITE_DONE: 
	{
		sqlite3_finalize(ptr);
		data = nullptr;
		return instance<>();
	}
	case SQLITE_MISUSE:
		throw stdext::exception("Statement already finalized");
	case SQLITE_ERROR:
		throw stdext::exception("SQl Error");
	case SQLITE_ROW:
	{
		auto count = sqlite3_column_count(ptr);
		auto res = instance<library::Map>::make();
		for (auto i = 0; i != count; i++)
		{
			auto tid = sqlite3_column_type(ptr, i);
			auto name = instance<std::string>::make(sqlite3_column_name(ptr, i));

			switch (tid)
			{
			case SQLITE_INTEGER:
				res->insert(name, instance<int64_t>::make(sqlite3_column_int64(ptr, i)));
				break;
			case SQLITE_FLOAT:
				res->insert(name, instance<double>::make(sqlite3_column_double(ptr, i)));
				break;
			case SQLITE_TEXT:
				res->insert(name, instance<std::string>::make((char*)sqlite3_column_text(ptr, i)));
				break;
			case SQLITE_NULL:
				res->insert(name, instance<>());
				break;
			default:
				break;
			}
		}
		return (instance<>)res;
	}

	default:
		return instance<bool>::make(false);
	}

	//
}

instance<Module> cultlang::sqlite3::make_sqlite3_bindings(instance<lisp::Namespace> ns, instance<> loader)
{
	auto ret = instance<Module>::make(ns, loader);
	auto sem = instance<CultSemantics>::make(ret);
	ret->builtin_setSemantics(sem);

	auto semantics = ret->require<CultSemantics>();
	
	lMM(sQL, [](t_str s) { auto res = t_db::make(*s); return res; });
	lMM(sQL"/exec", [](t_db db, t_str s, VarArgs<instance<>> args) {
		auto cmd = db->makeCommand(*s, args);
		cmd->step();
	});
	lMM(sQL"/command", [](t_db db, t_str s, VarArgs<instance<>> args) { 
		return db->makeCommand(*s, args);
	});
	
	lMM(sQL"/step", [](t_stmt st) {
		return st->step();
	});
	lMM(sQL"/fmap", [](t_stmt st, instance<PSubroutine> f) {
		auto res = instance<library::List>::make();
		instance<> ptr;
		while (ptr = st->step())
		{
			res->push(f->execute(f, { ptr }));
		}
		
		return res;
	});
	return ret;
}

BuiltinModuleDescription cultlang::sqlite3::BuiltinSqlite3("cult/sqlite3", cultlang::sqlite3::make_sqlite3_bindings);


#include "types/dll_entry.inc"
