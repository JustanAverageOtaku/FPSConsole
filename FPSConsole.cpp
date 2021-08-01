// FPSConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <cmath>
#include <utility>

using namespace std::chrono_literals;
const double pi = 3.14159265358979323846;

const int screenwidth = 120;
const int screenheight = 40;

float DegreeToRadians(float angleindegrees)
{
    return angleindegrees * pi / (float)180;
}

bool InBound(float& posx, float& posy)
{
    return posx > 0 && posx < 16 && posy > 0 && posy < 16;
}

int main()
{
    HANDLE screen = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    DWORD dw = 0;

    wchar_t screenbuffer[screenwidth * screenheight];
    for (int i = 0; i < screenwidth * screenheight; i++) screenbuffer[i] = L' ';
    
    std::wstring map;
    
    map += L"################";
    map += L"#..............#";
    map += L"####...........#";
    map += L"#..............#";
    map += L"#...........####";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"################";

    screenbuffer[(screenwidth - 1) * (screenheight - 1)] = L'\0';

    int fov = 60;
    float playerposx = 8;
    float playerposy = 4;
    int depth = 15;
    int wallheight = screenheight / 2;
    float movespeed = 2.f;
    float viewangle = 90;
    float angleshiftingfactor = fov / (float)screenwidth;
    float verticalshiftfactor = 16 / (float)screenheight;
    float horizontalshiftfactor = 16 / (float)screenwidth;
    std::pair<float, float> viewcollisions[screenwidth];
    //std::pair<float, float> viewnormal(cosf(viewangle * pi / 180), sinf(viewangle * pi / 180));
    //float rotationfactor = 0;

    auto tpi = std::chrono::system_clock::now();
    auto tpf = std::chrono::system_clock::now();

    SetConsoleActiveScreenBuffer(screen);
    while (1) 
    {
        tpf = std::chrono::system_clock::now();
        std::chrono::duration<float> chronoelapsedtime = tpf - tpi;
        float elapsedtime = chronoelapsedtime.count();
        tpi = tpf;
        
        //std::this_thread::sleep_for(50ms);

        if (GetAsyncKeyState(VK_RIGHT)) 
        {
            viewangle -= 5.f * 1.9f * elapsedtime;
        }
        else if (GetAsyncKeyState(VK_LEFT)) 
        {
            viewangle += 5.f * 1.9f * elapsedtime;
        }
        
        if (GetAsyncKeyState(VK_UP)) 
        {
            playerposy += -sinf(viewangle) * verticalshiftfactor * elapsedtime * movespeed;
            playerposx += cosf(viewangle) * horizontalshiftfactor * elapsedtime * movespeed;
        }
        else if (GetAsyncKeyState(VK_DOWN)) 
        {
            playerposy += sinf(viewangle) * verticalshiftfactor * elapsedtime * movespeed;
            playerposx += -cosf(viewangle) * horizontalshiftfactor * elapsedtime * movespeed;
        }
        
        if (GetAsyncKeyState(VK_RETURN)) 
        {
            break;
        }

        /*
        if ((int)viewangle == 180)
        {
            std::cout << "Crossed 180" << std::endl;
            std::this_thread::sleep_for(1000ms);
        }
        */
        
        float viewanglerightedge = viewangle - fov / 2;
        float angledge = viewanglerightedge - angleshiftingfactor;

        for (int i = 0; i < screenwidth; i++) 
        {
            angledge += angleshiftingfactor;
            float curxdepth = playerposx;
            float curydepth = playerposy;
            // float distance = 0;
            //std::cout << "Here" << std::endl;

            while (InBound(curxdepth, curydepth) && map[(int)curydepth * 16 + (int)curxdepth] != L'#')
            {
                curxdepth += cosf(DegreeToRadians(angledge)) * horizontalshiftfactor; //(angledge < 180) ? -cosf(DegreeToRadians(fmod(angledge, 90))) * verticalshiftfactor : cosf(DegreeToRadians(fmod(angledge, 90))) * verticalshiftfactor;
                curydepth += -sinf(DegreeToRadians(angledge)) * verticalshiftfactor;
                //distance++;

                //std::cout << "x depth: " << curxdepth << " " << -cosf(DegreeToRadians(60)) * verticalshiftfactor << std::endl;
                //std::cout << "y depth: " << curydepth << std::endl;
                if (GetAsyncKeyState(VK_RETURN))
                {
                    return 0;
                }

                if (map[(int)curydepth * 16 + (int)curxdepth] == L'#') 
                {
                    //std::cout << "Here" << std::endl;
                    float distance = sqrtf(powf(playerposx - curxdepth, 2) + powf(playerposy - curydepth, 2));
                    int heightofwall = wallheight * 4 / (float)distance; //wallheight * (wallheight / (distance * 18.75f));
                    int cielingheight = (screenheight - heightofwall) / 2;
                    viewcollisions[i].first = curxdepth;
                    viewcollisions[i].second = curydepth;
                    //int wallheightindex = (heightofwall < screenheight) ? (screenheight / 2) - heightofwall / 2.f : 0;

                    for (int k = 0; k < screenheight; k++)
                    {
                        // cieling
                        if (k <= cielingheight)
                        {
                            screenbuffer[screenwidth * k + (screenwidth - (i + 1))] = L' ';
                            continue;
                        }
                        // floor
                        if (k >= (cielingheight + heightofwall)) 
                        {
                            screenbuffer[screenwidth * k + (screenwidth - (i + 1))] = L'-';
                            continue;
                        }

                        if (distance <= 4) 
                        {
                            screenbuffer[screenwidth * k + (screenwidth - (i + 1))] = 0x2593; // Light
                        }
                        else if (distance <= 8) 
                        {
                            screenbuffer[screenwidth * k + (screenwidth - (i + 1))] = 0x2592; // Medium
                        }
                        else if (distance > 8) 
                        {
                            screenbuffer[screenwidth * k + (screenwidth - (i + 1))] = 0x2591; // Dark
                        }
                    }

                }
            }
            
            //WriteConsoleOutputCharacter(screen, screenbuffer, screenwidth * screenheight, { 0,0 }, &dw);
        }

        //swprintf_s(screenbuffer, 50, L"PlayerposX: %d, PlayerposY: %d", (int)playerposx, (int)playerposy);
        int row = 0;
        for (int p = 0; p < 256; p++) 
        {
            row += (p % 16 == 0 && p != 0) ? 1 : 0;
            screenbuffer[screenwidth * row + (p % 16)] = map[p];
        }
        for (int l = 0; l < screenwidth; l++) 
        {
            screenbuffer[screenwidth * (int)viewcollisions[l].second + (int)viewcollisions[l].first] = L'X';
        }
        screenbuffer[screenwidth * (int)playerposy + (int)playerposx] = L'P';
        WriteConsoleOutputCharacter(screen, screenbuffer, screenwidth * screenheight, { 0,0 }, &dw);
        //WriteConsoleOutputCharacter(screen, screenbuffer, screenwidth * screenheight, { 0,0 }, &dw);
    }
    
    //float thing =  fmod(120.5, 60);
    //std::cout << thing << std::endl;
    //std::cout << sin(pi) << std::endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
