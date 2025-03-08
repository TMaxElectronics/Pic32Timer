for /l %%x in (1, 1, 12) do (

echo "#ifdef T%%xCON" >> "Timer.c"
echo "   void __ISR(_TIMER_%%x_VECTOR) T%%xISR(){" >> "Timer.c"
echo "      IFS0CLR = _IFS0_T%%xIF_MASK;">> "Timer.c"
echo "      TMR_isrHandler(%%x);">> "Timer.c"
echo "   }" >> "Timer.c"
echo "#endif" >> "Timer.c"
echo. >> "Timer.c"
echo. >> "Timer.c"

)