#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include "filters.h" // Incluye el archivo de encabezado "filters.h"
#include <thread>
#include <unistd.h>
#include <fstream>
#include <map>
#include <variant>
#include <functional>

using namespace std;

int main(int argc, char *argv[])
{
    if (string(argv[1]) == "-help")
    {
        cout << "Uso: ./main <filtro> <nthreads> <[p1]> <img1> <custom_output> <[p2]> <img2>" << endl;
        return 0;
    }

    // Definición del mapa de las funciones

    map<string, function<ppm(map<string, VariantArg> &)>> functionMap;
    functionMap["plain"] = plain;
    functionMap["blackWhite"] = blackWhite;
    functionMap["contrast"] = contrast;
    functionMap["brightness"] = brightness;
    functionMap["shades"] = shades;
    functionMap["mergeFilter"] = mergeFilter;
    functionMap["boxBlur"] = boxBlur;
    functionMap["edgeDetection"] = edgeDetection;
    functionMap["sharpen"] = sharpen;

    // Se reciben los parámetros pasados desde main.cpp y se procesan
    // Después se guardan en un mapa que almacena claves en string y valores de tipos variantes.

    string filterName = string(argv[1]);
    unsigned int nthreads = atoi(argv[2]);
    float p1 = atof(argv[3]);
    string imgStr1(argv[4]);
    string out = string(argv[5]);
    string imgStr2(argv[7]);

    ppm img1(imgStr1);
    ppm img2(imgStr2);

    map<string, VariantArg> argsMap;

    function<ppm(map<string, VariantArg> &)> chosenFilter = functionMap.at(filterName);

    if (nthreads == 1)
    {
        argsMap["p1"] = p1;
        argsMap["out"] = out;
        argsMap["img1"] = img1;
        argsMap["img2"] = img2;
        ppm imgResult = chosenFilter(argsMap);
        imgResult.write(out);
    }
    else if (nthreads > 1)
    {
        thread threads[nthreads];
        
        map<string, VariantArg> argsMaps[nthreads];

        ppm imgResult = img1;

        if (filterName == "merge")
        {
            vector<ppm> imgs2perThread = threadsImageDivision(img2, nthreads);

            for (unsigned int threadIndex = 0; threadIndex < nthreads; threadIndex++)
            {
                argsMaps[threadIndex]["img2"] = imgs2perThread[threadIndex];
            }
        }

        vector<ppm> imgs1PerThread = threadsImageDivision(img1, nthreads);

        // unsigned int resPixel = img1.width % nthreads;
        // int threadWidth = img1.width / nthreads;
        // g++ -g main.cpp -o main

        for (unsigned int threadIndex = 0; threadIndex < nthreads; threadIndex++)
        {
            argsMaps[threadIndex]["p1"] = p1;
            argsMaps[threadIndex]["out"] = out;
            argsMaps[threadIndex]["img1"] = imgs1PerThread[threadIndex];

            // int initialPosX = threadIndex < resPixel ? threadIndex * (threadWidth + 1) : threadIndex * threadWidth;

            threads[threadIndex] = thread(applyFilterPerThread, chosenFilter, ref(argsMaps[threadIndex]), ref(imgResult), threadIndex);
        }

        for (unsigned int threadIndex = 0; threadIndex < nthreads; threadIndex++)
        {
            threads[threadIndex].join();
        }

        imgResult.write(out);
    }
    else
    {
        cout << "Sos un boludo qué querés que te diga flaco.";
    }

    cout << "Listo" << endl;
    return 0;
}

/*
Error. Unable to open SHELL=/bin/bash
Segmentation fault (core dumped)
*/

/*
==53610== Memcheck, a memory error detector
==53610== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==53610== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==53610== Command: ./main
==53610== 
terminate called after throwing an instance of 'std::logic_error'
  what():  basic_string::_M_construct null not valid
==53610== 
==53610== Process terminating with default action of signal 6 (SIGABRT)
==53610==    at 0x4C33A7C: __pthread_kill_implementation (pthread_kill.c:44)
==53610==    by 0x4C33A7C: __pthread_kill_internal (pthread_kill.c:78)
==53610==    by 0x4C33A7C: pthread_kill@@GLIBC_2.34 (pthread_kill.c:89)
==53610==    by 0x4BDF475: raise (raise.c:26)
==53610==    by 0x4BC57F2: abort (abort.c:79)
==53610==    by 0x490EBBD: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==53610==    by 0x491A24B: ??? (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==53610==    by 0x491A2B6: std::terminate() (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==53610==    by 0x491A517: __cxa_throw (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==53610==    by 0x4911389: std::__throw_logic_error(char const*) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
==53610==    by 0x10D363: void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct<char const*>(char const*, char const*, std::forward_iterator_tag) (basic_string.tcc:212)
==53610==    by 0x10D3E8: std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string<std::allocator<char> >(char const*, std::allocator<char> const&) (basic_string.h:539)
==53610==    by 0x10B9BF: main (main.cpp:17)
==53610== 
==53610== HEAP SUMMARY:
==53610==     in use at exit: 72,914 bytes in 3 blocks
==53610==   total heap usage: 4 allocs, 1 frees, 72,946 bytes allocated
==53610== 
==53610== LEAK SUMMARY:
==53610==    definitely lost: 0 bytes in 0 blocks
==53610==    indirectly lost: 0 bytes in 0 blocks
==53610==      possibly lost: 144 bytes in 1 blocks
==53610==    still reachable: 72,770 bytes in 2 blocks
==53610==                       of which reachable via heuristic:
==53610==                         stdstring          : 66 bytes in 1 blocks
==53610==         suppressed: 0 bytes in 0 blocks
==53610== Rerun with --leak-check=full to see details of leaked memory
==53610== 
==53610== For lists of detected and suppressed errors, rerun with: -s
==53610== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
Aborted (core dumped)

*/
 
/*
==54213== Memcheck, a memory error detector
==54213== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==54213== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==54213== Command: ./main blackWhite 3 1 imgs/cebra.ppm out/cebra_blackwhite.ppm 1 imgs/chihiro.ppm
==54213== 
==54213== Invalid read of size 4
==54213==    at 0x10F61A: ppm (ppm.h:35)
==54213==    by 0x10F61A: _Uninitialized<ppm&> (variant:263)
==54213==    by 0x10F61A: _Variadic_union<ppm&> (variant:358)
==54213==    by 0x10F61A: _Variadic_union<1, ppm&> (variant:363)
==54213==    by 0x10F61A: _Variadic_union<2, ppm&> (variant:363)
==54213==    by 0x10F61A: _Variadic_union<3, ppm&> (variant:363)
==54213==    by 0x10F61A: _Variant_storage<3, ppm&> (variant:418)
==54213==    by 0x10F61A: _Copy_ctor_base<3, ppm&> (variant:528)
==54213==    by 0x10F61A: _Move_ctor_base<3, ppm&> (variant:556)
==54213==    by 0x10F61A: _Copy_assign_base<3, ppm&> (variant:616)
==54213==    by 0x10F61A: _Move_assign_base<3, ppm&> (variant:674)
==54213==    by 0x10F61A: _Variant_base<3, ppm&> (variant:726)
==54213==    by 0x10F61A: variant<3, ppm&> (variant:1457)
==54213==    by 0x10F61A: variant<ppm&> (variant:1428)
==54213==    by 0x10F61A: std::enable_if<((__exactly_once<std::variant_alternative<__accepted_index<ppm&>, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> >::type>)&&(is_constructible_v<std::variant_alternative<__accepted_index<ppm&>, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> >::type, ppm&>))&&(is_assignable_v<std::variant_alternative<__accepted_index<ppm&>, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> >::type&, ppm&>), std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>&>::type std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>::operator=<ppm&>(ppm&) (variant:1491)
==54213==    by 0x10C753: main (main.cpp:89)
==54213==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
==54213== 
==54213== 
==54213== Process terminating with default action of signal 11 (SIGSEGV)
==54213==  Access not within mapped region at address 0x0
==54213==    at 0x10F61A: ppm (ppm.h:35)
==54213==    by 0x10F61A: _Uninitialized<ppm&> (variant:263)
==54213==    by 0x10F61A: _Variadic_union<ppm&> (variant:358)
==54213==    by 0x10F61A: _Variadic_union<1, ppm&> (variant:363)
==54213==    by 0x10F61A: _Variadic_union<2, ppm&> (variant:363)
==54213==    by 0x10F61A: _Variadic_union<3, ppm&> (variant:363)
==54213==    by 0x10F61A: _Variant_storage<3, ppm&> (variant:418)
==54213==    by 0x10F61A: _Copy_ctor_base<3, ppm&> (variant:528)
==54213==    by 0x10F61A: _Move_ctor_base<3, ppm&> (variant:556)
==54213==    by 0x10F61A: _Copy_assign_base<3, ppm&> (variant:616)
==54213==    by 0x10F61A: _Move_assign_base<3, ppm&> (variant:674)
==54213==    by 0x10F61A: _Variant_base<3, ppm&> (variant:726)
==54213==    by 0x10F61A: variant<3, ppm&> (variant:1457)
==54213==    by 0x10F61A: variant<ppm&> (variant:1428)
==54213==    by 0x10F61A: std::enable_if<((__exactly_once<std::variant_alternative<__accepted_index<ppm&>, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> >::type>)&&(is_constructible_v<std::variant_alternative<__accepted_index<ppm&>, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> >::type, ppm&>))&&(is_assignable_v<std::variant_alternative<__accepted_index<ppm&>, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> >::type&, ppm&>), std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>&>::type std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>::operator=<ppm&>(ppm&) (variant:1491)
==54213==    by 0x10C753: main (main.cpp:89)
==54213==  If you believe this happened as a result of a stack
==54213==  overflow in your program's main thread (unlikely but
==54213==  possible), you can try to increase the size of the
==54213==  main thread stack using the --main-stacksize= flag.
==54213==  The main thread stack size used in this run was 8388608.
==54213== 
==54213== HEAP SUMMARY:
==54213==     in use at exit: 9,110,307 bytes in 1,726 blocks
==54213==   total heap usage: 3,235 allocs, 1,509 frees, 11,563,727 bytes allocated
==54213== 
==54213== LEAK SUMMARY:
==54213==    definitely lost: 0 bytes in 0 blocks
==54213==    indirectly lost: 0 bytes in 0 blocks
==54213==      possibly lost: 0 bytes in 0 blocks
==54213==    still reachable: 9,110,307 bytes in 1,726 blocks
==54213==         suppressed: 0 bytes in 0 blocks
==54213== Rerun with --leak-check=full to see details of leaked memory
==54213== 
==54213== For lists of detected and suppressed errors, rerun with: -s
==54213== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
Segmentation fault (core dumped)

*/

/*
/usr/bin/ld: /tmp/ccjMFsFp.o: in function `main':
/home/nicolas/Documents/TP2-Imagine/main.cpp:26: undefined reference to `plain(std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> > > >&)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:27: undefined reference to `blackWhite(std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> > > >&)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:28: undefined reference to `contrast(std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> > > >&)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:29: undefined reference to `brightness(std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> > > >&)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:30: undefined reference to `shades(std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> > > >&)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:31: undefined reference to `mergeFilter(std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> > > >&)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:32: undefined reference to `boxBlur(std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> > > >&)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:33: undefined reference to `edgeDetection(std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> > > >&)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:34: undefined reference to `sharpen(std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> > > >&)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:46: undefined reference to `ppm::ppm(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:47: undefined reference to `ppm::ppm(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:60: undefined reference to `ppm::write(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:72: undefined reference to `threadsImageDivision(ppm&, int)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:80: undefined reference to `threadsImageDivision(ppm&, int)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:93: undefined reference to `applyFilterPerThread(std::function<ppm (std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> > > >&)>, std::map<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm>, std::less<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const, std::variant<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, unsigned int, float, ppm> > > >&, ppm&, int)'
/usr/bin/ld: /home/nicolas/Documents/TP2-Imagine/main.cpp:101: undefined reference to `ppm::write(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)'
collect2: error: ld returned 1 exit status

*/