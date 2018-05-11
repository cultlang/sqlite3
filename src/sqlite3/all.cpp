#include "sqlite3/common.h"

#include "all.h"

#include "lisp/library/system/prelude.h"
#include "lisp/semantics/cult/calling.h"
#include "types/cpp/cpp_interface.h"

using namespace craft;
using namespace craft::lisp;
using namespace craft::types;


instance<Module> cultlang::sqlite3::make_sqlite3_bindings(instance<lisp::Namespace> ns, instance<> loader)
{
	auto ret = instance<Module>::make(ns, loader);
	auto sem = instance<CultSemantics>::make(ret);
	ret->builtin_setSemantics(sem);

	auto semantics = ret->require<CultSemantics>();
	

	ret->builtin_setSemantics(ret->require<CultSemantics>());
	return ret;
}

BuiltinModuleDescription cultlang::sqlite3::BuiltinSqlite3("cult.sqlite3", cultlang::sqlite3::make_sqlite3_bindings);

#include "types/dll_entry.inc"
