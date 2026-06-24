@echo off
chcp 65001 > nul

echo Compilando o jogo...

gcc src/main.c -o main.exe -I dep/include -L dep/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer

if %errorlevel% equ 0 (
    echo.
    echo =======================================
    echo  Compilação concluída com SUCESSO!
    echo =======================================
    echo.
    echo Executando o jogo...
    echo ---------------------------------------
    main.exe
) else (
    echo.
    echo [ERRO] Ocorreu um problema na compilação. Verifique as mensagens acima.
)

echo.
pause