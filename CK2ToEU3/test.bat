set i=%~n1
set j=%~n2
set old_path=%~3

echo Testing %i% with the %j% configuration
copy "%old_path%\CK2_Saves\%i%.zip" .\%i%.zip

"%SEVENZIP_LOC%\7z.exe" e -tzip "%i%.zip" "*.*" -mx5
del %i%.zip
call CK2ToEU3.exe %i%.ck2
del *.ck2 /q
copy *.eu3 "%old_path%\testresults\%j%\"
del *.eu3 /q
copy log.txt "%old_path%\testresults\%j%\%i%-Log.txt"
del log.txt /q
xcopy .\%i%\* "%old_path%\testresults\%j%\%i%\"  /E
del %i% /q /s
rmdir %i% /s /q