#include <iostream>
#include <unordered_map>

#include "Bicycle/Win/Process/Process.h"
#include "ArgParse/ArgumentParser.h"

#define ADD_TO_MAP(MAP,NAME) (MAP[L#NAME] = NAME)

using namespace std;

int wmain(int argc, wchar_t *argv[ ], wchar_t /* *envp[ ] */)
{
     unordered_map<wstring,unsigned long> creationFlagDict;
     ADD_TO_MAP(creationFlagDict, CREATE_NEW_CONSOLE);
     ADD_TO_MAP(creationFlagDict, CREATE_NO_WINDOW);
     ADD_TO_MAP(creationFlagDict, CREATE_UNICODE_ENVIRONMENT);

     unordered_map<wstring,HWND> insertAfterDict;
     ADD_TO_MAP(insertAfterDict, HWND_BOTTOM);
     ADD_TO_MAP(insertAfterDict, HWND_NOTOPMOST);
     ADD_TO_MAP(insertAfterDict, HWND_TOP);
     ADD_TO_MAP(insertAfterDict, HWND_TOPMOST);

     unordered_map<wstring,UINT> windowPosFlagsDict;
     ADD_TO_MAP(windowPosFlagsDict, SWP_DRAWFRAME);
     ADD_TO_MAP(windowPosFlagsDict, SWP_FRAMECHANGED);
     ADD_TO_MAP(windowPosFlagsDict, SWP_HIDEWINDOW);
     ADD_TO_MAP(windowPosFlagsDict, SWP_NOACTIVATE);
     ADD_TO_MAP(windowPosFlagsDict, SWP_NOCOPYBITS);
     ADD_TO_MAP(windowPosFlagsDict, SWP_NOMOVE);
     ADD_TO_MAP(windowPosFlagsDict, SWP_NOOWNERZORDER);
     ADD_TO_MAP(windowPosFlagsDict, SWP_NOREDRAW);
     ADD_TO_MAP(windowPosFlagsDict, SWP_NOREPOSITION);
     ADD_TO_MAP(windowPosFlagsDict, SWP_NOSENDCHANGING);
     ADD_TO_MAP(windowPosFlagsDict, SWP_NOSIZE);
     ADD_TO_MAP(windowPosFlagsDict, SWP_NOZORDER);
     ADD_TO_MAP(windowPosFlagsDict, SWP_SHOWWINDOW);

     unordered_map<wstring,int> showWindowDict;
     ADD_TO_MAP(showWindowDict, SW_FORCEMINIMIZE);
     ADD_TO_MAP(showWindowDict, SW_HIDE);
     ADD_TO_MAP(showWindowDict, SW_MAXIMIZE);
     ADD_TO_MAP(showWindowDict, SW_MINIMIZE);
     ADD_TO_MAP(showWindowDict, SW_SHOW);
     ADD_TO_MAP(showWindowDict, SW_FORCEMINIMIZE);
     ADD_TO_MAP(showWindowDict, SW_SHOWDEFAULT);
     ADD_TO_MAP(showWindowDict, SW_SHOWMAXIMIZED);
     ADD_TO_MAP(showWindowDict, SW_SHOWMINIMIZED);
     ADD_TO_MAP(showWindowDict, SW_SHOWMINNOACTIVE);
     ADD_TO_MAP(showWindowDict, SW_SHOWNA);
     ADD_TO_MAP(showWindowDict, SW_SHOWNOACTIVATE);
     ADD_TO_MAP(showWindowDict, SW_SHOWNORMAL);

     ArgParse::ArgumentParser<wchar_t> parser;
     auto path= parser.addPositional<wstring,1,1>(L"path");
     auto cmdLine= parser.addPositional<wstring>(L"cmdLine");
     auto program= parser.addOptional<wstring>(L"-an",L"--app_name",L"/an");
     auto workDir= parser.addOptional<wstring>(L"-wd",L"--work_dir",L"/wd");

     auto creationFlags=
         parser.addOptional<wstring,'+'>(L"-cf",L"--creation_flags",L"/cf");

     auto interval=
         parser.addOptional<unsigned long,1,1>(L"-i",L"--interval",L"/i");
     interval = 1000UL; // ms

     auto detach= parser.addOptional<bool,1,1>(L"-d",L"--detach",L"/d");

     auto insertAfter=
         parser.addOptional<wstring,1,1>(L"-ia",L"--insert_after",L"/ia");
     insertAfter = L"HWND_TOP";

     auto showWindow=
         parser.addOptional<wstring,1,1>(L"-sw",L"--show_window",L"/sw");

     auto windowPosFlags =
         parser.addOptional<wstring,'+'>(L"-wpf",L"--window_pos_flags",L"/wpf");

     auto wait = parser.addOptional<unsigned long>(L"-w",L"--wait",L"/w");
     wait = INFINITE;

     auto showStdOut= parser.addOptional<bool>(L"-so",L"--show_stdout",L"/so");
     auto help= parser.addOptional<wstring>(L"-h",L"--help",L"/?");

     auto position = parser.addOptional<int,2,2>(L"-p",L"--position",L"/p");
     auto size_ = parser.addOptional<int,2,2>(L"-s",L"--size",L"/s");

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

         if(program.exists())
           process.setAppName(*program);

         if(cmdLine.exists())
           process.setCmdLine(*cmdLine);

         if(workDir.exists())
           process.setCurrentDir(*workDir);

         if(creationFlags.exists())
         {
           unsigned long flags=
               accumulate(begin(*creationFlags),end(*creationFlags),0ul,
             [&creationFlagDict](unsigned long l, const wstring& flagName)
             {
                return l | creationFlagDict.at(flagName);
             });
           process.setCreationFlags(flags);
         }

         process.start();
         cout<<"Process started."<<endl;

         while(!process.isFinished())
         {
           if(process.isStarted() && !process.isFinished())
           {
              HWND window=
                  Bicycle::findWindowById(process.threadId(),process.processId());
              if(window)
              {
                HWND rootWnd= GetAncestor(window,GA_ROOT);
                if(rootWnd)
                {
                   auto insertAfterMode = HWND_TOP;
                   if(insertAfter.exists())
                     insertAfterMode = insertAfterDict.at(*insertAfter);

                   unsigned int flags= SWP_NOMOVE | SWP_NOSIZE;
                   if(position.exists())
                     flags &= ~SWP_NOMOVE;

                   if(size_.exists())
                     flags &= ~SWP_NOSIZE;

                   if(windowPosFlags.exists())
                   {
                     flags =
                       accumulate(begin(*windowPosFlags),end(*windowPosFlags),
                                  flags,
                                  [&windowPosFlagsDict](auto l,const auto& s)
                     {
                       return l | windowPosFlagsDict.at(s);
                     });
                   }

                   RECT rect;
                   GetWindowRect(rootWnd,&rect);

                   int x = position.exists() ? (*position)[0] : rect.left;
                   int y = position.exists() ? (*position)[1] : rect.top;

                   int w = position.exists() ? (*size_)[0] : rect.right-rect.left;
                   int h = position.exists() ? (*size_)[1] : rect.bottom-rect.top;

                   SetWindowPos(rootWnd, insertAfterMode, x,y, w,h, flags);
                }

                if(showWindow.exists())
                {
                  ShowWindow(rootWnd,showWindowDict.at(*showWindow));
                }

                wcout << L"Setted" << endl;
                break;
              }
              else
                wcout << L"Fail" << endl;

              Sleep(*interval);
           }
         }

         if(detach.exists() && detach.value())
            process.detach();
         else
         {
           process.waitForFinished(*wait);
           cout<<"Process finished with exit code #"<< process.exitCode() <<endl;
         }
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
     return 0;
}
