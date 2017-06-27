if "%1" == "clean" goto clean
	call vs10-win64.bat || exit /b
	cmake --build . --target ALL_BUILD --config Debug || exit /b
	cmake --build . --target INSTALL --config Debug || exit /b
	cmake --build . --target ALL_BUILD --config Release || exit /b
	cmake --build . --target INSTALL --config Release || exit /b
goto end
:clean	
	cmake --build vs10-win64\ --target clean --config Debug
	cmake --build vs10-win64\ --target clean --config Release
	del vs10-win64\CMakeCache.txt
:end
pause
