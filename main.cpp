#include <iostream>
#include <unordered_map>

#include "Bicycle/Win/Process/Process.h"
#include "ArgParse/ArgumentParser.h"

using namespace std;

int wmain(int argc, wchar_t *argv[ ], wchar_t /* *envp[ ] */)
{
     unordered_map<wstring,unsigned long> creationFlagDict =
     {
       { L"CREATE_NEW_CONSOLE", CREATE_NEW_CONSOLE },
       { L"CREATE_NO_WINDOW", CREATE_NO_WINDOW },
       { L"CREATE_UNICODE_ENVIRONMENT", CREATE_UNICODE_ENVIRONMENT },
       { L"CREATE_DEFAULT_ERROR_MODE", CREATE_DEFAULT_ERROR_MODE },

       { L"CREATE_BREAKAWAY_FROM_JOB", CREATE_BREAKAWAY_FROM_JOB },
       { L"CREATE_NEW_PROCESS_GROUP", CREATE_NEW_PROCESS_GROUP },
       { L"CREATE_PROTECTED_PROCESS", CREATE_PROTECTED_PROCESS },
       { L"CREATE_PRESERVE_CODE_AUTHZ_LEVEL", CREATE_PRESERVE_CODE_AUTHZ_LEVEL },
       { L"CREATE_SECURE_PROCESS", CREATE_SECURE_PROCESS },
       { L"CREATE_SEPARATE_WOW_VDM", CREATE_SEPARATE_WOW_VDM },
       { L"CREATE_SUSPENDED", CREATE_SUSPENDED },
       { L"DEBUG_ONLY_THIS_PROCESS", DEBUG_ONLY_THIS_PROCESS },
     };

     ArgParse::ArgumentParser<wchar_t> parser;
     auto path= parser.addPositional<wstring,1,1>(L"path");
     auto cmdLine= parser.addPositional<wstring>(L"cmdLine");
     auto program= parser.addOptional<wstring>(L"-p",L"--program",L"/p");
     auto workDir= parser.addOptional<wstring>(L"-wd",L"--work_dir",L"/wd");

     auto creationFlags = parser.addOptional<wstring,'+'>(L"-cf",L"--creation_flags",L"/cf");
     auto wait = parser.addOptional<unsigned long>(L"-w",L"--wait",L"/w");
     auto windowMode = parser.addOptional<wstring,'+'>(L"-wm",L"--window_mode",L"/wm");

     auto detach= parser.addOptional<bool>(L"-d",L"--detach",L"/d");
     auto showStdOut= parser.addOptional<bool>(L"-so",L"--show_stdout",L"/so");
     auto help= parser.addOptional<wstring>(L"-h",L"--help",L"/?");

     try
     {
       parser.parseArgs(argc,argv);

       for(auto a: parser.positionals())
         wcout << a->name()<< ":\t"<<a->valueAsString() << endl;
       for(auto a: parser.optionals())
         wcout << a->name()<< ":\t"<<a->valueAsString() << endl;
     }
     catch(const ArgParse::Exception<wchar_t>& e)
     {
       wcerr<< L"Error: " << e.what()       << endl << endl;
       wcerr<< L"Usage: " << parser.usage() << endl;
       wcerr<< L"Help:\n" << parser.help()  << endl;
       return 1;
     }

     if(help)
     {
       wcerr<< L"Usage: " << parser.usage() << endl;
       wcerr<< L"Help:\n" << parser.help()  << endl;
       return 0;
     }

     try
     {
         Bicycle::Process process;

         if(program)
           process.setAppName(*program);

         if(cmdLine)
           process.setCmdLine(*cmdLine);

         if(workDir)
           process.setCurrentDir(*workDir);

         if(creationFlags.exists() && creationFlags.hasValue())
         {
           unsigned long flags=
               accumulate(begin(*creationFlags),end(*creationFlags),0ul,
             [&creationFlagDict](unsigned long l, const wstring& flagName)
             {
                return l+creationFlagDict.at(flagName);
             });
           process.setCreationFlags(flags);
         }

         process.start();

         //process.waitForInputIdle(); // Ждем готовности калькулятора
         cout<<"Process ready."<<endl;

         if(wait)
         {
           Sleep(*wait);

           if(process.isStarted() && !process.isFinished())
           {
             auto id = GetCurrentThreadId();
             vector<HWND> windows;
             if(Bicycle::threadWindows(id,windows))
             {
               wcout<< L"size:" << windows.size() <<  endl;
               for(size_t i=0; i<windows.size(); ++i)
                 if(IsWindow(windows[i]))
                 {
                   HWND rootWnd= GetAncestor(windows[i],GA_ROOT);
                   if(rootWnd)
                   {
                     long dwExStyle = GetWindowLong(rootWnd, GWL_EXSTYLE);
                     bool topMost= (dwExStyle & WS_EX_TOPMOST);
                     ::SetWindowPos(rootWnd,
                                    topMost?HWND_NOTOPMOST:HWND_TOPMOST,
                                    -1,-1,-1,-1,
                                    SWP_NOSIZE|SWP_NOMOVE);
                     break;
                   }
                 }
             }
             else
               wcout<< L"bad!" <<  endl;
           }
         }
         else
         {
           process.waitForFinished();
           cout<<"Process finished with exit code #"<< process.exitCode() <<endl;
         }


         // Ждем 5 сек и закрываем программу
         //Sleep(5000);
         // process.closeWindows(); // Шлем WM_CLOSE окну
         process.waitForFinished(); // Ждем завершения калькулятора
     }
     catch(const Bicycle::Exception& e)
     {
         SetConsoleCP(1251);
         SetConsoleOutputCP(1251);
         cerr<< e.message()<<endl;
     }
     catch(const exception& e)
     {
         cerr<< e.what()<<endl;
     }
     getchar();
     return 0;
}
