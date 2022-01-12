@echo off

$bin\ZarroKey\Release\ZarroKey.exe printver > zarrokey.ver

@echo Update to server:
pscp -l zarro "$bin\ZarroKey\Release\ZarroKey.exe" "zarrokey.ver" "$bin\Release\Setup.msi" byte-it.in:/var/www/html

del zarrokey.ver >nul 2>nul
