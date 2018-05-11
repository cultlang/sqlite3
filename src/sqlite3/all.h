#pragma once
#include "sqlite3/common.h"

namespace cultlang {
namespace sqlite3 {
	extern craft::lisp::BuiltinModuleDescription BuiltinSqlite3;

	CULTLANG_SQLITE3_EXPORTED craft::instance<craft::lisp::Module> make_sqlite3_bindings(craft::instance<craft::lisp::Namespace> ns, craft::instance<> loader);
}}
