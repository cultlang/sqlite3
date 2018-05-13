#include "sqlite3/common.h"

#include "replxx/replxx.hxx"
#include "lisp/semantics/cult/cult.h"
#include "lisp/library/system/prelude.h"
#include "lisp/features/repl.h"

using namespace craft;
using namespace craft::lisp;


void ensure_dlls()
{
#ifdef _WIN32
	auto target = path::dir(path::executable_path());
	auto files = path::list_files(target);
	for (auto f : files)
	{
		auto ext = f.substr(f.find('.') + 1);
		if (ext == "dll")
		{
			craft::types::load_dll(path::join(target, f));
		}
	}
#endif
}

int main(int argc, char** argv) {
	craft::types::boot();
	ensure_dlls();
	features::LispRepl r;
	
	
	while (true)
	{
		try
		{
			auto eval = r.step();
			std::cout << eval.toString() << "\n";
		}
		catch (features::ReplExitException e)
		{
			break;
		}
		catch (features::ReplParseException e)
		{
			std::cout << "Parse Error: " << e.what() << "\n";
		}
		catch (stdext::exception e)
		{
			std::cout << e.what() << "\n";
		}
		catch (std::exception e)
		{
			std::cout << "Unhandled Internal Exception: " << e.what() << "\n";
		}
	}
}