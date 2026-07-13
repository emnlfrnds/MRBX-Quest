echo off

ECHO Tentando compilação...

gcc main.c -o main.exe -lwinmm

if %ERRORLEVEL% EQU 0 (
    ECHO =================================
    ECHO Compilado com sucesso!
    ECHO Incializando jogo.
    ECHO =================================
    start main.exe
) else (
    ECHO ERRO DE COMPILAÇÂO! Veja as mensagens acima.
)