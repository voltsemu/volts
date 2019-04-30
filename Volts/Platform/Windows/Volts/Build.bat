echo %1
cd %1..\..\..\..\Build && ninja && python move.py