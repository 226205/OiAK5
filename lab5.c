#include <stdio.h>
#include <stdlib.h>

struct wektor
{
    float wek[4];
};

float wekSum(struct wektor v)
{
    return v.wek[0] + v.wek[1] + v.wek[2] + v.wek[3];
}

float det(struct wektor macierz[4])
{
    struct wektor result;
/* Korzystamy ze wzoru z wyciaganiem przed nawias:   detM = 
	(a0*b1-a1*b0)*(c2*d3-c3*d2) +
	(a2*b0-a0*b2)*(c1*d3-c3*d1) +
	(a0*b3-a3*b0)*(c1*d2-c2*d1) +
	(a1*b2-a2*b1)*(c0*d3-c3*d0) +
	(a2*b3-a3*b2)*(c0*d1-c1*d0) +
	(a3*b1-a1*b3)*(c0*d2-c2*d0)
*/
    asm(			
			//pierwsze mnozenie w pierwszych 4 linijkach w pierwszym nawiasie kolejno od dolu aby zachowac analogie w drugiej czesci programu. W pierwszej czesci nie ma to znaczenia dla wyniku, natomaist druga czesc bedzie w wartosci ujemnej i trzeba ja bedzie odjac
            "movups (%0), %%xmm0 \n"	
            "movups (%1), %%xmm1 \n"
            "shufps $0x48, %%xmm0, %%xmm0 \n" //a1 a0 a2 a0  ->  01 00 10 00 = 4 8
            "shufps $0xb1, %%xmm1, %%xmm1 \n" //b2 b3 b0 b1  ->  10 11 00 01 = 11 1 (b1)
            "mulps %%xmm0, %%xmm1 \n"
			//drugie mnozenie w pierwszych 4 linijkach  w pierwszym nawiasie kolejno od dolu
            "movups (%0), %%xmm0 \n"	
            "movups (%1), %%xmm2 \n"
            "shufps $0xb1, %%xmm0, %%xmm0 \n" //a2 a3 a0 a1  ->  10 11 00 01 = 11 1 (b1)
            "shufps $0x48, %%xmm2, %%xmm2 \n" //b1 b0 b2 b0  ->  01 00 10 00 = 4 8
            "mulps %%xmm0, %%xmm2 \n"

			//odejmowanie pomnozonych elementów w pierwszych nawiasach w pierwszych 4 linijkach, wynik zachowujemy w xmm2
            "subps %%xmm1, %%xmm2 \n"	

			//pierwsze mnozenie w drugich nawiasach w kolejnosci od dolu
            "movups (%2), %%xmm0 \n"	
            "movups (%3), %%xmm1 \n"
            "shufps $0x16, %%xmm0, %%xmm0 \n" //c0 c1 c1 c2  ->  00 01 01 10 = 1 6
            "shufps $0xef, %%xmm1, %%xmm1 \n" //d3 d2 d3 d3  ->  11 10 11 11 = 14 15 (ef)
            "mulps %%xmm0, %%xmm1 \n"
			//drugie mnozenie w drugich nawiasach w kolejnosci od dolu
            "movups (%2), %%xmm0 \n"		
            "movups (%3), %%xmm3 \n"
            "shufps $0xef, %%xmm0, %%xmm0 \n" //c3 c2 c3 c3  ->  11 10 11 11 = 14 15 (ef)
            "shufps $0x16, %%xmm3, %%xmm3 \n" //d0 d1 d1 d2  ->  00 01 01 10 = 1 6
            "mulps %%xmm0, %%xmm3 \n"

			//odejmowanie pomnożonych wartości w drugich nawiasach, wynik zachowujemy w xmm3
            "subps %%xmm1, %%xmm3 \n" 

			//wykonanie mnożenia pierwszych i drugich nawiasów w pierwszych 4 wierszach
            "mulps %%xmm2, %%xmm3 \n" 
			
		//Pierwsze 4 linijki zrobione, do zrobienia ostatnie 2. Analogicznie trzeba dorobic jeszcze 2 linijki rownan z zerami, żeby wszystkie mogly te operacje wykonać dwukrotnie. Wynik pierwszych 4 linijek przechowywany w xmm3
// Wszystkie operacje wykonujemy analogicznie jak powyzej

            "movups (%0), %%xmm0 \n"
            "movups (%1), %%xmm2 \n"
            "shufps $0xe, %%xmm0, %%xmm0 \n" //a3 a2  ->  11 10 = 0 14 (0e)
            "shufps $0x7, %%xmm2, %%xmm2 \n" //b1 b3  ->  01 11 = 0 7
            "mulps %%xmm0, %%xmm2 \n"
            
            "movups (%0), %%xmm0 \n"
            "movups (%1), %%xmm4 \n"
            "shufps $0x7, %%xmm0, %%xmm0 \n" //a1 a3  ->  01 11 = 0 7
            "shufps $0xe, %%xmm4, %%xmm4 \n" //b3 b2  ->  11 10 = 0 14 (0e)
            "mulps %%xmm0, %%xmm4 \n"

            "subps %%xmm4, %%xmm2 \n"  // Odejmujemy pomnozone elementy w pierwszysch nawiasach i chowamy wynik w xmm2, ktore juz zwolnilismy wczesniej liczac xmm3

            "movups (%2), %%xmm0 \n"
            "movups (%3), %%xmm4 \n"
            "shufps $0x0, %%xmm0, %%xmm0 \n" //c0 c0  ->  00 00 = 0 0
            "shufps $0x9, %%xmm4, %%xmm4 \n" //d2 d1  ->  10 01 = 0 9
            "mulps %%xmm0, %%xmm4 \n"

            "movups (%2), %%xmm0 \n"
            "movups (%3), %%xmm5 \n"
            "shufps $0x9, %%xmm0, %%xmm0 \n" //c2 c1  ->  10 01 = 0 9
            "shufps $0x0, %%xmm5, %%xmm5 \n" //d0 d0  ->  00 00 = 0 0
            "mulps %%xmm5, %%xmm0 \n"

            "subps %%xmm4, %%xmm0 \n"  // Odejmujemy pomnozone elementy w drugich nawiasach i chowamy wynik w xmm0

            "mulps %%xmm0, %%xmm2 \n" // Mnozenie pierwszych i drugich nawiasow w dolnych wierszach, wynik do xmm2
            "subps %%xmm2, %%xmm3 \n" // Odejmujemy zamiast dodawania jak we wzorze, gdyz mamy wynik drugiej czesci w wartosci przeciwnej
            "movups %%xmm3, (%4) \n"
            :
            :"r"(macierz[0].wek), "r"(macierz[1].wek), "r"(macierz[2].wek), "r"(macierz[3].wek),
             "r"(result.wek)
    );
    
    return wekSum(result);
}

int main()
{
    struct wektor macierz[4];

    float mm[4][4] = {{1,7,15,3},{2,13,7,4},{13,8,1,3},{4,6,11,4}};

    for(int i=0; i < 4; ++i)
   		for(int j=0; j < 4; ++j)
            		macierz[i].wek[j] = mm[i][j]; // przypisanie macierzy do tablicy 2d

    float result = det(macierz);

    printf("Wyznacznik macierzy jest rowny: %f \n", result);
}
