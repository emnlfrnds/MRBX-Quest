echo off

gcc main.c -o main.exe -lwinmm

if %ERRORLEVEL% EQU 0 (
    ECHO Compilado com sucesso!
    start main.exe
) else (
    ECHO ERRO DE COMPILAÇÂO! Veja as mensagens acima.
)