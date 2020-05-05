Pri testovani sme zistili problem na Intel grafickej karte, ktory sposobuje ze aj po
vycisteni framebuffer nevycisti. Na Nvidia karte sme tento problem nezaznamenali.

Pre zostavenie je potrebne mat nainstalovany CMake, ktory je mozne stiahnut z tohto linku:
https://cmake.org/download/

Dalej je potrebny prekladac ktory podporuje OpenMP a C++20 (napr. GCC, Clang, Intel).
Program bol testovany s Clangom, ktory je mozne stiahnut odtialto:
https://releases.llvm.org/download.html

Dalej je potrebne stiahnut a rozbalit kniznice SFML a GLM:
https://github.com/g-truc/glm/releases
https://www.sfml-dev.org/download/sfml/2.5.1/

Ked je vsetko prichystane je mozne zostavit projekt (testovane pre Visual Studio) naslednovne:
1. spustit cmake-gui
2. Where is the source code: nastavit na adresar v ktorom sa nachadza subor CMakeLists.txt
3. vytvorit adresar build
4. Where to build the binaries: nastavit na vytvoreny build adresar
5. stlacit Configure (ak si pyta generator vybrat napr Visual Studio)
6. nastavit SFML_DIR na cestu <rozbalene SFML>/lib/cmake/SFML
7. ak glm_DIR nie je zobrazene stlacit este raz Configure
8. nastavit glm_DIR na cestu <rozbalene GLM>/cmake/glm
9. stlacit Generate
10. stlacit Open Project a potom je mozne zostavit projekt vo Visual Studiu
