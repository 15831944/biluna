FOR %%d IN (db acc crm mrp pcalc peng sail scan srm bil) DO ECHO %%d && cd ..\%%d\src && C:\Qt\5.4\msvc2013_64_opengl\bin\qmake.exe && C:\Qt\Tools\QtCreator\bin\jom.exe clean && cd ..\..\scripts