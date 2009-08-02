@echo on

cd /d %~dp0 || goto end

(cd 3rd && call build.bat) || goto end

cd /d %~dp0

(cd build && call build.bat)

:end
exit /b
