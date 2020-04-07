#pragma once

#include "main.h"
#include "asm.cpp"

void SendCMD(const char *cmd)
{
#ifdef DEBUG
	printf("Sending %s\n", cmd);
#endif
	char* newcmd = (char*)cmd;
	size_t size = strlen(cmd);
	wpm(PVOID(cmdptr + 0x728 + 0x2004), 1, &size);
	wpm(PVOID(cmdptr + 0x728), size, static_cast<void*>(newcmd));
}


const auto sqrtss = [](float in)
{
	__m128 reg = _mm_load_ss(&in);
	return _mm_mul_ss(reg, _mm_rsqrt_ss(reg)).m128_f32[0];
};


void menu()
{
	SetLayeredWindowAttributes(hWnd, NULL, NULL, NULL);
	DwmEnableBlurBehindWindow(hWnd, &bb);

	ID3DXFont* pFont;
	D3DXCreateFont(p_Device, 20, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);

	DrawString((char*)"Spy External Multihax", 5, 5, 25, 255, 255, 255, pFont);

	if (drawmenu == 1) {
		DrawFilledRectangle(20, 300, 280, 350 + cheat.Count() * 25, 255, 7, 231, 171);
		DrawFilledRectangle(25, 330, 275, 345 + cheat.Count() * 25, 100, 10, 45, 41);

		DrawString((char*)"Spy External Multihax", 65, 305, 255, 10, 45, 41, pFont);

		int menutop = 340;
		for (byte i = 0; i < cheat.Count(); i++)
		{
			if (menuselect == i) {
				color = D3DCOLOR_ARGB(255, 255, 255, 255);
			}
			else if (cheat(i).name == "Disable All & Exit")
			{
				color = D3DCOLOR_ARGB(255, 255, 255, 50);
			}
			else {
				color = D3DCOLOR_ARGB(255, 7, 231, 175);
			}

			DrawString((char*)cheat(i).name.c_str(), 35, menutop, color, pFont);

			if (cheat(i).name != "Speedhack")
			{
				if (cheat(i).enabled < 10)
					DrawString((char*)(std::to_string(cheat(i).enabled).c_str()), 255, menutop, color, pFont);
				else
					DrawString((char*)(std::to_string(cheat(i).enabled).c_str()), 255 - 4, menutop, color, pFont);
			}
			else
			{
				DrawString((char*)(std::to_string((float)cheat(i).enabled / (float)10.f).c_str()), 248, menutop, 3, color, pFont);
			}
			menutop += 25;
		}
	}
	pFont->Release();
}

void WH() {
	if (cheat("Chameleon Wallhack") == 1) {
		SpyJmp(PVOID(d3d9_dll_base + dip9 + 0xCE), asmWHcave, 0);
		SendCMD("cl_ragdoll_physics_enable 0; cl_minmodels 1; cl_min_ct 3; cl_min_t 3");
	}
	if (cheat("Chameleon Wallhack") == 0) {
		byte bytes[] = { 0xFF, 0xD7, 0x83, 0xC4, 0x1c };
		wvm(PVOID(d3d9_dll_base + dip9 + 0xCE), sizeof(bytes), bytes);
		SendCMD("cl_ragdoll_physics_enable 1; cl_minmodels 0");

		byte bytes2[] = { 0xEB, 0x05 };
		wvm(PVOID((DWORD)whlight + 0xA), sizeof(bytes2), bytes2);
	}

	if (cheat("Chameleon Wallhack") == 2) {
		byte bytes3[] = { 0x90, 0x90 };
		wvm(PVOID((DWORD)whlight + 0xA), sizeof(bytes3), bytes3);
	}
}

void Namestealer() {

	DWORD steamidptr;
	TCHAR name[32];

	int rando, old = 65;
	while (true) {

		if (cheat("Namestealer") == 1) {

			rando = rand() % 64;
			rando -= 1;

			rvm(PVOID(0x24000000 + 0x39D4FC), 1, &myid);
			if (rando + 2 == (int)myid)
				continue;

			if (rando == old)
				continue;

			steamidptr = rpm(engine_dll_base + 0x3958C8);
			steamidptr = rpm(steamidptr + 0x38);

			if (steamidptr == 0)
			{
				steamidptr = rpm(engine_dll_base + 0x3958A8);
				steamidptr = rpm(steamidptr + 0x38);
				steamidptr = rpm(steamidptr + 0x24);
				steamidptr = rpm(steamidptr + 0x14 + (0x28 * (rando + 1)));

				name[0] = 0x0;

				rvm(PVOID(steamidptr), 32, &name);
			}
			else
			{
				steamidptr = rpm(steamidptr + 0x24);
				steamidptr = rpm(steamidptr + 0x14 + (0x28 * (rando + 1)));

				name[0] = 0x0;

				rvm(PVOID(steamidptr), 32, &name);
			}

			if ((int)name[0] > 0x20 || (int)name[0] < 0)
			{
#ifdef DEBUG
				std::cout << rando << " set name to " << name << std::endl;
#endif
				char towrite[64] = "setinfo name \u0022";
				strcat_s(towrite, name);
				strcat_s(towrite, " \u0022");
				SendCMD(towrite);

				old = rando;
				Sleep(500);
			}
		}
		Sleep(10);
	}
}

#include <bitset>
void Bunnyhop()
{
	#define FORCE_JUMP_BITMASK (1<<0)
	std::bitset<8> dwForceJumpBitMask{ 0b0000'0001 };
	int currJumpState{ 0 };

	float VisY, VisYnew;
	float VisYd;
	while (true)
	{
		if (cheat("Bunnyhop & Autostrafe") == 1 && tWnd == GetForegroundWindow() && GetAsyncKeyState(VK_SPACE) < 0)
		{
			rvm(PVOID(0x24000000 + 0x3FD550), 4, &VisY); //read Y
			VisYnew = VisY;

			wpm(0x24000000 + 0x3E71E4, 6); //jump
			while (cheat("Bunnyhop & Autostrafe") == 1 && tWnd == GetForegroundWindow() && GetAsyncKeyState(VK_SPACE) < 0)
			{
				rvm(PVOID(0x24000000 + 0x3FD550), 4, &VisYnew); //read new Y

				if (VisY != VisYnew) //Y changed?
				{
					boostsleep = 1;
					VisYd = 1 * (VisY - VisYnew); //5

					if (cheat("Spinbot & AntiAim") == 0) {
						if (VisYd < 0.0f)
						{
							wpm(0x24000000 + 0x3E7250, 0); //-moveright
							wpm(0x24000000 + 0x3E725C, 1); //+moveleft
						}
						else
						{
							wpm(0x24000000 + 0x3E7250, 1); //+moveright 
							wpm(0x24000000 + 0x3E725C, 0); //-moveleft
						}
					}

					else
					{
						if (VisYd < 0.0f)
						{
							wpm(0x24000000 + 0x3E7250, 1); //+moveright
							wpm(0x24000000 + 0x3E725C, 0); //-moveleft
						}
						else
						{
							wpm(0x24000000 + 0x3E7250, 0); //-moveright 
							wpm(0x24000000 + 0x3E725C, 1); //+moveleft
						}
					}

					wpm(engine_dll_base + 0x39541c + 4 - enginedelta, 1.0f); //reset Z angle
					VisY = VisYnew; //set New Y as old Y
				}

				rvm( PVOID(0x24000000 + 0x3E71E4), sizeof(currJumpState), &currJumpState);
				if (rpm(0x24000000 + 0x3E9FE4)) //onGround?
				{
					currJumpState |= FORCE_JUMP_BITMASK;
					wpm( PVOID(0x24000000 + 0x3E71E4), sizeof(currJumpState), &currJumpState);
#ifdef DEBUG
					cout << "jumping\n";
#endif
				}
				else 
				{
					currJumpState &= ~FORCE_JUMP_BITMASK;
					wpm( PVOID(0x24000000 + 0x3E71E4), sizeof(currJumpState), &currJumpState);
				}

				Sleep(1);
			}
			wpm(0x24000000 + 0x3E7250, 0); //-moveright 
			wpm(0x24000000 + 0x3E725C, 0); //-moveleft
			wpm(engine_dll_base + 0x39541c + 4 - enginedelta, 1.0f); //reset Z angle
		}


		if (cheat("Bunnyhop & Autostrafe") == 2 && tWnd == GetForegroundWindow() && GetAsyncKeyState(VK_SPACE) < 0)
		{
			wpm(0x24000000 + 0x3E71A8, 5); //duck
			wpm(0x24000000 + 0x3E71E4, 6);
			while (cheat("Bunnyhop & Autostrafe") == 2 && tWnd == GetForegroundWindow() && GetAsyncKeyState(VK_SPACE) < 0)
			{
				if (rpm(0x24000000 + 0x3EA03C)) //onGround
				{
					wpm(0x24000000 + 0x3E71E4, 6);
				}
				Sleep(1);
			}
			wpm(0x24000000 + 0x3E71A8, 4); //unduck
		}

		Sleep(1);
	}
}

void Flyhack()
{
	flycave = DWORD(SpyInject(Fly, LPVOID(0x24000000 + 0xB841B)));
	float vispos[3], flyangX, flyangY;
	while (true) {
		if (cheat("Visual Flyhack") == 1) {
			rvm((PVOID)(localplayer + 0x29C), 12, &vispos);
			while (cheat("Visual Flyhack") == 1) {
				rvm(PVOID(0x24000000 + 0x3FD550 - 4), 4, &flyangX);
				rvm(PVOID(0x24000000 + 0x3FD550), 4, &flyangY);

				flyangY -= 90.0;
				if (GetAsyncKeyState(0x57) < 0) //W
				{
					vispos[0] += -sin(flyangY * PI / 180);
					vispos[1] += cos(flyangY * PI / 180);
					wpm((LPVOID)(localplayer + 0x29C), 4, &vispos[0]);
					wpm((LPVOID)(localplayer + +0x29C + 0x4), 4, &vispos[1]);
					vispos[2] += -sin(flyangX * PI / 180);
					wpm((LPVOID)(localplayer + +0x29C + 0x8), 4, &vispos[2]);
				}

				if (GetAsyncKeyState(0x53) < 0) //S
				{
					vispos[0] -= -sin(flyangY * PI / 180);
					vispos[1] -= cos(flyangY * PI / 180);
					wpm((LPVOID)(localplayer + 0x29C), 4, &vispos[0]);
					wpm((LPVOID)(localplayer + +0x29C + 0x4), 4, &vispos[1]);
					vispos[2] += sin(flyangX * PI / 180);
					wpm((LPVOID)(localplayer + +0x29C + 0x8), 4, &vispos[2]);
				}

				if (GetAsyncKeyState(0x41) < 0) //A
				{
					vispos[0] -= cos(flyangY * PI / 180);
					vispos[1] -= sin(flyangY * PI / 180);

					wpm((LPVOID)(localplayer + 0x29C), 4, &vispos[0]);
					wpm((LPVOID)(localplayer + +0x29C + 0x4), 4, &vispos[1]);
				}

				if (GetAsyncKeyState(0x44) < 0) //D
				{
					vispos[0] += cos(flyangY * PI / 180);
					vispos[1] += sin(flyangY * PI / 180);

					wpm((LPVOID)(localplayer + 0x29C), 4, &vispos[0]);
					wpm((LPVOID)(localplayer + +0x29C + 0x4), 4, &vispos[1]);
				}
				Sleep(1);
			}
		}
		Sleep(1);
	}
}

void Spinbot()
{
	float visX;
	float visY;

	float visYd = 0;

	freevisangX = SpyInject(FreeVisualAnglesX, PVOID(0x2415D32E));
	freevisangY = SpyInject(FreeVisualAnglesY, PVOID(0x2415D3A0));
	freevisangZ = SpyInject(FreeVisualAnglesZ, PVOID(0x2415D412));

	fixPredict = SpyInject(FixPredict, PVOID(0x240D46F9));

	SpyInjectAndJump(ZetToZero, PVOID(0x24000000 + 0x192B0), 4);

	wpm(0x12300, 180.0f);
	wpm(0x12304, -180.0f);
	wpm(0x12308, 360.0f);

	while (true)
	{
		if (cheat("Spinbot & AntiAim") == 1)
		{
			SpyJmp(PVOID(0x24000000 + 0xF85A4), rotating, 5);			
			wpm(0x12334, 45.0f); //set spinhack speed
			wpm(0x12345, 90.0f); //set X ang
			wpm(0x12220, 1); //semaphore
			while (cheat("Spinbot & AntiAim") == 1) {
				
				if (GetAsyncKeyState(VK_LBUTTON) < 0)
				{	
					if (tWnd == GetForegroundWindow())
					wpm(0x12204, 1);
				}
				else {
					wpm(0x12204, 0);
					rvm(PVOID(0x12345), 4, &visX);
					if (visX != 90.0f && visX != -90.0f)
					wpm(0x12345, 90.0f);
				}

				if (GetAsyncKeyState(0x57) < 0 || GetAsyncKeyState(0x53) < 0) //w/s
				{
					visYd = 90.0f;
					if (GetAsyncKeyState(0x57) < 0 && GetAsyncKeyState(0x41) < 0) //w+a
						visYd -= 45.0f;
					else
						if (GetAsyncKeyState(0x53) < 0 && GetAsyncKeyState(0x41) < 0) //s+a
							visYd -= -45.0f;

					if (GetAsyncKeyState(0x57) < 0 && GetAsyncKeyState(0x44) < 0) //w+d
						visYd -= -45.0f;
					else
						if (GetAsyncKeyState(0x53) < 0 && GetAsyncKeyState(0x44) < 0) //s+d
							visYd -= 45.0f;
					wpm(0x12330, visYd);
				}
				else
				{
					if (GetAsyncKeyState(0x41) < 0) //a
					{
						visYd = 0;
						wpm(0x12330, visYd);
					}
					if (GetAsyncKeyState(0x44) < 0) //d
					{
						visYd = 180;
						wpm(0x12330, visYd);
					}
				}

				wpm(engine_dll_base + 0x39541c + 4 - enginedelta, 1.0f); //z
				Sleep(1);
			}
			wpm(0x12204, 1);

			wpm(0x12334, 0); //rotation to 0
			wpm(0x1234D, 0); //z to 0

			byte bytes2[] = { 0x8b, 0x4c, 0x24, 0x14, 0x89, 0x0b, 0x8b, 0x54, 0x24, 0x18 };
			wvm(PVOID(0x24000000 + 0xF85A4), sizeof(bytes2), bytes2);
		}

		if (cheat("Spinbot & AntiAim") == 2) //UPSIDE-DOWN
		{
			wpm(DWORD(freevisangX) + 1, 0xA13E9090);
			wpm(0x12345, 179.99f);
			while (cheat("Spinbot & AntiAim") == 2)
			{
				if (GetAsyncKeyState(VK_LBUTTON) == 0)
				{
					wpm(0x12204, 0);

					rvm(PVOID(0x24000000 + 0x3FD54C + 4), 4, &visY);
					visY += 180.0f;
					if (visY > 180.0f) visY -= 360.0f;
					if (visY < -180.0f) visY += 360.0f;
					wpm(0x12349, visY);

					rvm(PVOID(0x12345), 4, &visX);
					if (visX != 178.3999939f && visX != 178.5f)
						wpm(0x12345, 178.3999939f);
				}
				else
					if (tWnd == GetForegroundWindow())
						wpm(0x12204, 1);

				Sleep(1);
			}
			wpm(0x12204, 1);
			wpm(DWORD(freevisangX) + 1, 0xA13E2AEB);
		}

		if (cheat("Spinbot & AntiAim") == 3) //EXPERIMENTAL
		{
			wpm(0x12345, 0); //reset X
			byte bytes1[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
			wvm(PVOID(engine_dll_base + 0x42727 + enginedelta), sizeof(bytes1), bytes1); //comment for burst-fire
			byte bytes2[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
			wvm(PVOID(engine_dll_base + 0x426D9 + enginedelta), sizeof(bytes2), bytes2);

			while (cheat("Spinbot & AntiAim") == 3)
			{
				if (GetAsyncKeyState(VK_LBUTTON) == 0)
				{
					rvm(PVOID(0x24000000 + 0x3FD54C + 4), 4, &visY);
					wpm(0x12349, visY);

					wpm(engine_dll_base + 0x3953bc - enginedelta, 22);
					wpm(engine_dll_base + 0x3953c0 - enginedelta, 31);
					wpm(svcheatsptr + 0x314, 1);
					wpm(timescaleptr + 0x108, 38.0f); //host_timescale
					wpm(engine_dll_base + 0x4D2860 - enginedelta, 12.0f); //host_framerate
					
					wpm(0x24000000 + 0x3E71E4, 5); //jump
				}
				else
					wpm(0x24000000 + 0x3E71E4, 4); //unjump
				
				Sleep(1);
			}

			dword2bytes dw2b3 = { engine_dll_base + 0x3953C0 - enginedelta };
			dword2bytes dw2b4 = { engine_dll_base + 0x3953bc - enginedelta };
			byte bytes3[] = { 0x83, 0x05, dw2b3.bytes[0],dw2b3.bytes[1],dw2b3.bytes[2],dw2b3.bytes[3], 0x01 };
			wvm(PVOID(engine_dll_base + 0x42727 + enginedelta), sizeof(bytes3), bytes3);
			byte bytes4[] = { 0xA3, dw2b4.bytes[0],dw2b4.bytes[1],dw2b4.bytes[2],dw2b4.bytes[3], 0xC7, 0x05, dw2b3.bytes[0],dw2b3.bytes[1],dw2b3.bytes[2],dw2b3.bytes[3], 0x00, 0x00, 0x00, 0x00 };
			wvm(PVOID(engine_dll_base + 0x426D9 + enginedelta), sizeof(bytes4), bytes4);

			wpm(svcheatsptr + 0x314, 0);
			wpm(timescaleptr + 0x108, 1.0f);
			wpm(engine_dll_base + 0x4D2860 - enginedelta, 0.0f); //host_framerate
			wpm(0x24000000 + 0x3E71E4, 4);
		}

		if (cheat("Spinbot & AntiAim") == 4) //BACKWARDS
		{

			while (cheat("Spinbot & AntiAim") == 4)
			{

				if (GetAsyncKeyState(VK_LBUTTON) == 0)
				{
					rvm(PVOID(0x24000000 + 0x3FD54C + 4), 4, &visY);
					visY += 180.0f;
					if (visY > 180.0f) visY -= 360.0f;
					if (visY < -180.0f) visY += 360.0f;

					wpm(0x12345, 88.0f);
					wpm(0x12349, visY);
				}
				Sleep(1);
			}

		}

		Sleep(1);
	}
}

void CalcAngle(float *src, float *dst, float *angles)
{
	double delta[3] = { (src[0] - dst[0]), (src[1] - dst[1]), (src[2] - dst[2]) };
	double hyp = sqrtss(delta[0] * delta[0] + delta[1] * delta[1]);
	angles[0] = (float)(atan(delta[2] / hyp) * 180.0 / 3.14159265);
	angles[1] = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
	angles[2] = 0.0f;

	if (delta[0] >= 0.0) angles[1] += 180.0f;
}

void Aimbot()
{
	byte four = 4, five = 5, six = 6;

	DWORD boneptr;
	float mycoords[3];
	float myangle[2];
	float newangle[2];
	float enemycoords[3];
	double delta[3];
	double hyp;

	bool aiming = 0;

	DWORD ecx, eax;
	int randomseed;
	int realseed;
	float realup, realright;

	float myang[2], newangg[2];
	float punch[2]; DWORD punchptr;

	float xx, yy;

	while (true)
	{
		if (angleshack && GetAsyncKeyState(VK_LBUTTON) == 0 && cheat("Spinbot & AntiAim") == 0 ||
			angleshack && cheat("No Recoil & Spread").enabled == 1 && cheat("Spinbot & AntiAim") == 0)
		{
			rvm(PVOID(0x24000000 + 0x3FD54C), 8, &myang);

			if (isnan(myang[0]))
			{
				myang[0] = 0;
				wpm(PVOID(0x24000000 + 0x3FD54C), 4, &myang[0]);
			}
			if (isnan(myang[1]))
			{
				myang[1] = 0;
				wpm(PVOID(0x24000000 + 0x3FD54C + 4), 4, &myang[1]);
			}
			wpm(PVOID(0x12345), 8, &myang);
			Sleep(1);
		}

		if (tWnd == GetForegroundWindow() && GetAsyncKeyState(VK_LBUTTON) < 0) {

			if (cheat("Aimbot") == 2)
				SetCursorPos(tSize.left + xl_closest_final, tSize.top + yl_closest_final);

			if (cheat("Aimbot") == 1) {
				rvm(PVOID(0x24000000 + 0x3FD5C4), 12, &mycoords);

				if (closest_final != -2) {
					rvm(PVOID(0x24000000 + 0x3BF1F4 + 0x10 * closest_final), 4, &boneptr);
					rvm(PVOID(boneptr + 0x24), 4, &boneptr);
					rvm(PVOID(boneptr + 0x34), 4, &boneptr);
					rvm(PVOID(boneptr + 0x158), 4, &boneptr);
					rvm((PVOID)(boneptr + 0x60 + 0x24C), 4, &enemycoords[0]);
					rvm((PVOID)(boneptr + 0x60 + 0x25C), 4, &enemycoords[1]);
					rvm((PVOID)(boneptr + 0x60 + 0x26C), 4, &enemycoords[2]);
					enemycoords[2] = enemycoords[2] + 2;

					CalcAngle(mycoords, enemycoords, newangle);

					if (newangle[1] < -180.0f)
						newangle[1] += 360.0f;
					else
						if (newangle[1] > 180.0f) newangle[1] -= 360.0f;

					if (!isnan(newangle[0])) {

						aiming = 1;

						if (cheat("Spinbot & AntiAim").enabled > 1 && cheat("No Recoil & Spread") != 1) //metka1
						{
							if (cheat("Spinbot & AntiAim") == 2)
							{
								newangle[0] = -(newangle[0] + 180.0f);
								newangle[1] = newangle[1] + 180.0f;
							}
							wpm(PVOID(0x12345), 8, &newangle);
						}
						if (!angleshack)
						{
							wpm(PVOID(engine_dll_base + 0x395418 - enginedelta), 8, &newangle);
						}
					}
				}
			}

			if (cheat("No Recoil & Spread") == 1)
			{
				if (tWnd == GetForegroundWindow() && GetAsyncKeyState(VK_LBUTTON) < 0)
				{
					if (aiming)
					{
						//readaimang
						myang[0] = newangle[0];
						myang[1] = newangle[1];
					}
					else
						rvm(PVOID(0x24000000 + 0x3FD54C), 8, &myang); //readvisang

					if (cheat("Spinbot & AntiAim") == 2) //metka2
					{
						myang[0] = -(myang[0] + 180.0f);
						myang[1] = myang[1] + 180.0f;
					}

					//read punch
					punchptr = rpm(0x24000000 + 0x3FB2F0);
					rvm(PVOID(punchptr + 0xBB0), 8, &punch);

					//read sequence
					rvm(PVOID(engine_dll_base + 0x3953c0 - enginedelta), 4, &ecx);
					rvm(PVOID(engine_dll_base + 0x3953bc - enginedelta), 4, &eax);
					randomseed = MD5_PseudoRandom(ecx + eax + 1) & 0x7FFFFFFF;
					randomseed &= 255;

					RandomSeed(randomseed + 1);
					xx = RandomFloat(-0.5f, 0.5f) + RandomFloat(-0.5f, 0.5f);
					yy = RandomFloat(-0.5f, 0.5f) + RandomFloat(-0.5f, 0.5f);

					if (cheat("Spinbot & AntiAim") != 2 && cheat("Spinbot & AntiAim") != 3)
					{
						newangg[0] = myang[0] + (yy * 1.9f) - punch[0] * 1.9f;
						newangg[1] = myang[1] + (xx * 1.9f) - punch[1] * 1.9f;
					}
					else
					{
						newangg[0] = myang[0] + (yy * 1.9f) - punch[0] * 1.9f;
						newangg[1] = myang[1] - (xx * 1.9f) - punch[1] * 1.9f;
					}

					if (newangg[0] > 180.0f) newangg[0] -= 360.0f;
					if (newangg[0] < -180.0f) newangg[0] += 360.0f;
					if (newangg[1] > 180.0f) newangg[1] -= 360.0f;
					if (newangg[1] < -180.0f) newangg[1] += 360.0f;

					wpm(PVOID(0x12345), 8, &newangg);

					//fire
					wpm((LPVOID)0x243E71D8, 1, &six);

					Sleep(40);
				}
			}

			if (GetAsyncKeyState(VK_LBUTTON) < 0 && cheat("No Recoil & Spread") != 1 && cheat("Spinbot & AntiAim") != 0)
			{
				if (!aiming)
				{
					rvm(PVOID(0x24000000 + 0x3FD54C), 8, &myang);
					if (cheat("Spinbot & AntiAim") == 2) //metka3
					{
						myang[0] = -(myang[0] + 180.0f);
						myang[1] = myang[1] + 180.0f;
					}
				}
				else
				{
					myang[0] = newangle[0];
					myang[1] = newangle[1];

				}
				wpm(PVOID(0x12345), 8, &myang);

			}

			if (cheat("No Recoil & Spread") == 2 && cheat("Spinbot & AntiAim") != 3 )
			{
				//wpm(svcheatsptr + 0x314, 1);
				wpm(timescaleptr + 0x108, 2.0f); 
				wpm(engine_dll_base + 0x4D2860 - enginedelta, 200.0f); //host_framerate

				//set sequence
				wpm(engine_dll_base + 0x3953bc - enginedelta, 0x6e);
				wpm(engine_dll_base + 0x3953c0 - enginedelta, 6);

				//fire
				wpm((LPVOID)0x243E71D8, 1, &six);

				Sleep(5);

				//wpm(svcheatsptr + 0x314, 0);
				wpm(timescaleptr + 0x108, 1.0f);
				wpm(engine_dll_base + 0x4D2860 - enginedelta, 0.0f); //host_framerate
				Sleep(10); 
			}
			aiming = 0;

			Sleep(5);
		}
		Sleep(1);
	}
}


void noSmokeFlash() {
	if (cheat("No Smoke & Flash") == 1) {
		wpm(0x24000000 + 0x3E9C34, 0); //r_drawparticles 0
		SpyInjectAndJump(NoSmoke, PVOID(0x24000000+0x8E4F3), 0);
		BYTE twobytes[] = { 0x90, 0xE9 };
		wpm(PVOID(0x24000000 + 0x1D1D5D), 2, &twobytes);
	}
	if (cheat("No Smoke & Flash") == 0) {
		wpm(0x24000000 + 0x3E9C34, 1); //r_drawparticles 1
		BYTE twobytes[] = { 0x0F, 0x8B };
		wpm(PVOID(0x24000000 + 0x1D1D5D), 2, &twobytes);
		BYTE fivebytes[] = { 0x8B, 0x01, 0xFF, 0x50, 0x04 };
		wpm(PVOID(0x24000000 + 0x8E4F3), sizeof(fivebytes), &fivebytes); //0x24000000 + 0x1D1D4A
	}
#ifdef DEBUG
	std::cout << "NoSmoke & NoFlash triggered\n";
#endif
}

void noHandsSky() {
	if (cheat("No Hands & Sky") == 1) {
		wpm(0x24000000 + 0x3EEDFC, 0);
		wpm(0x24000000 + 0x3EE78C, 1);

		wpm(svcheatsptr + 0x314, 1);
		SendCMD("mat_suppress \"models/weapons/v_models/hands/v_hands.vmt\" ");
		Sleep(500);
		wpm(svcheatsptr + 0x314, 0);
	}

	if (cheat("No Hands & Sky") == 0) {
		wpm(0x24000000 + 0x3EEDFC, 1);
		wpm(0x24000000 + 0x3EE78C, 0);

		wpm(svcheatsptr + 0x314, 1);
		SendCMD("mat_suppress \"models/weapons/v_models/hands/v_hands.vmt\" ");
		Sleep(500);
		wpm(svcheatsptr + 0x314, 0);
	}
}


bool WorldToScreen(float viewmatrix[][4], float coords[], float *x, float *y, float *w) {

	*x = viewmatrix[0][0] * coords[0] + viewmatrix[0][1] * coords[1] + viewmatrix[0][2] * coords[2] + viewmatrix[0][3];
	*y = viewmatrix[1][0] * coords[0] + viewmatrix[1][1] * coords[1] + viewmatrix[1][2] * coords[2] + viewmatrix[1][3];
	*w = viewmatrix[3][0] * coords[0] + viewmatrix[3][1] * coords[1] + viewmatrix[3][2] * coords[2] + viewmatrix[3][3];

	if (*w < 0.1f)
		return false;

	*x = *x / *w;
	*y = *y / *w;

	*x = (Width / 2 * *x) + (*x + Width / 2);
	*y = -(Height / 2 * *y) + (*y + Height / 2);

	return 1;
}


void timer() {
	float c4timer = rpm(0x22000000 + 0x5A4A7C);
#ifdef DEBUG
	printf("c4timer = %f\n", c4timer);
#endif
	chrono::system_clock::time_point mStartedTime = chrono::system_clock::now();
	for (;; Sleep(10)) {
		chrono::system_clock::time_point mElapsedTime = chrono::system_clock::now();
		std::chrono::duration<float> diff = mElapsedTime - mStartedTime;
		bomb = c4timer - diff.count();
		xd = 5.60 * (bomb * 100 / c4timer);
		if (!bombplanted) break;
	}
	bomb = 0.00f;
	xd = 0;
}


void myDraw() {

		menu();
		if (rpm(engine_dll_base + 0x53B55C - enginedelta) != 0) {
			if (cheat("Radarhack & Bombtimer") == 2) {
				rvm(PVOID(0x24000000 + 0x3BA3C0), 4, &radarhackptr);
				if (radarhackptr)
					wpm(PVOID(radarhackptr + 0x13D8), 64, &sf);
			}

			if (cheat("Radarhack & Bombtimer") == 1) {
				sprite->Begin(D3DXSPRITE_ALPHABLEND);
				sprite->Draw(tex, NULL, NULL, &position, color2);
				sprite->End();
			}

			if (cheat("Aimbot").enabled > 0) {
				aimfov = cheat("Aimbot FOV").enabled * 5;
				DrawCircle(Width / 2, Height / 2, aimfov, 0, 360, D3DCOLOR_ARGB(50, 255, 255, 0));
			}

			rvm(PVOID(0x24000000 + 0x3B51C4), 4, &localplayer);
			rvm(PVOID(localplayer + 0x90), 1, &myteam);

			if (myteam == 2) myteam = 64;
			if (myteam == 3) myteam = 255;

			if (cheat("Smart Crosshair") == 1) {

				rvm(PVOID(0x243E7208), 1, &who);
				switch (who)
				{
				case 1:
					color = D3DCOLOR_ARGB(100, 0, 255, 0);
					break;
				case 2:
					color = D3DCOLOR_ARGB(100, 255, 0, 0);
					break;
				default:
					color = D3DCOLOR_ARGB(100, 0, 0, 255);
					break;
				}

				wpm((LPVOID)0x243E7208, 1, &zero);

				scrCenterX = Width / 2;
				scrCenterY = Height / 2;

				D3DRECT rect1 = { scrCenterX - 13,  scrCenterY - 2,  scrCenterX + 13, scrCenterY + 2 };
				D3DRECT rect2 = { scrCenterX - 2,  scrCenterY - 13,  scrCenterX + 2, scrCenterY + 13 };

				IDirect3DDevice9_Clear(p_Device, 1, &rect1, D3DCLEAR_TARGET, color, 0, 0);
				IDirect3DDevice9_Clear(p_Device, 1, &rect2, D3DCLEAR_TARGET, color, 0, 0);
			}

			if (cheat("Aimbot").enabled > 0 || cheat("Radarhack & Bombtimer").enabled == 1 || cheat("ESP").enabled > 0)
			{

				rvm(PVOID(0x24000000 + 0x3FD5C4), 4, &myposX);
				rvm(PVOID(0x24000000 + 0x3EE0C8), 4, &myposY);
				rvm(PVOID(0x24000000 + 0x3FD550), 4, &myangY);

				rvm(PVOID(0x24000000 + 0x3FD5CC), 4, &myposZ);

				myangY -= 90;

				yl_closest = 1000; xl_closest = 1000;


				if (cheat("ESP") == 3) {
					ID3DXFont* pFont;
					D3DXCreateFont(p_Device, 12, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);
					color = D3DCOLOR_XRGB(255, 255, 255);
					offs = 0;

					maxentityid = rpm(0x24000000 + 0x3CF208);
					for (i = 0; i <= maxentityid; i++)
					{
						rvm(PVOID(0x24000000 + 0x3BF1D4 + 0x10 * i), 4, &boneptr);
						rvm(PVOID(boneptr + 0x4A8), 4, &boneptr);

						rvm(PVOID(boneptr + 0x0c), 4, &coords[0]);
						rvm(PVOID(boneptr + 0x1c), 4, &coords[1]);
						rvm(PVOID(boneptr + 0x2c), 4, &coords[2]);

						if (WorldToScreen(viewmatrix, coords, &xl, &yl, &wl)) {

							ss << hex << i;
							s = ss.str();

							entity = rpm(0x24000000 + 0x3CF20C + 8 * i);
							entity = rpm(entity);
							entity = rpm(entity - 0x4);
							entity = rpm(entity - 0x8);
							espwep[0] = 0x0;
							rvm(PVOID(entity + 0x8), 24, &espwep);

							if ((char*)espwep[0] == 0x0) //Unknown entity
							{
								ss.str("");
								continue;
							}
							s += ": ";

							if (abs(prevX - xl) < 5 && abs(prevY - yl) < 5)
							{
								DrawString(cstr, xl, yl + offs * 10, color, pFont);
								DrawString((char*)(espwep), xl + 20, yl + offs * 10, color, pFont);
								offs++;
							}
							else
							{
								DrawString(cstr, xl, yl, color, pFont);
								DrawString((char*)(espwep), xl + 20, yl, color, pFont);
							}

							prevX = xl; prevY = yl;
							ss.str("");

						}
					}
					pFont->Release();
				}
				for (i = -1; i < 63; i++)
				{
					rvm(PVOID(0x24000000 + 0x39D4FC), 1, &myid);
					if (i + 2 == (int)myid)
						continue;

					if (cheat("Aimbot") != 2)
						rvm((PVOID)(ptr + 0x228 + 0x30 + i * 0x140), 12, &coords);
					else {
						rvm(PVOID(0x24000000 + 0x3BF1F4 + 0x10 * i), 4, &boneptr);

						if (!boneptr) {
							coords[0] = 0; coords[1] = 0; coords[2] = 0;
						}
						else {
							rvm(PVOID(boneptr + 0x24), 4, &boneptr);
							rvm(PVOID(boneptr + 0x34), 4, &boneptr);


							if (!boneptr) {
								coords[0] = 0; coords[1] = 0; coords[2] = 0;
							}
							else {

								rvm(PVOID(boneptr + 0x158), 4, &boneptr);

								if (!boneptr) {
									coords[0] = 0; coords[1] = 0; coords[2] = 0;
								}
								else {
									rvm((PVOID)(boneptr + 0x60 + 0x24C), 4, &coords[0]);
									rvm((PVOID)(boneptr + 0x60 + 0x25C), 4, &coords[1]);
									rvm((PVOID)(boneptr + 0x60 + 0x26C), 4, &coords[2]);

									coords[2] = coords[2] + 1;
								}
							}
						}
					}

					rvm((PVOID)(ptr + 0x228 + 0x30 + i * 0x140), 12, &radarcoords);
					rvm(PVOID(ptr + offset + 0x2c + i * PLRSZ), 4, &hp);
					rvm(PVOID(ptr + offset + 0x6 + i * PLRSZ), 1, &team);

					if (radarcoords[0] != 0 && hp > 0) {
						//Radar trigonometry
						deltaX = myposX - radarcoords[0];
						deltaY = myposY - radarcoords[1];

						deltaZ = myposZ - coords[2];

						deltaXold = deltaX; deltaYold = deltaY;

						deltaX = deltaXold * cos(myangY * PI / 180) + deltaYold * sin(myangY* PI / 180);
						deltaY = -deltaXold * sin(myangY* PI / 180) + deltaYold * cos(myangY* PI / 180);

						if (sqrtss(pow(deltaX, 2) + pow(deltaY, 2)) > 1500) {
							float k = 1500 / sqrtss(pow(deltaX, 2) + pow(deltaY, 2));
							deltaX = k * deltaX;
							deltaY = k * deltaY;
						}

						if (cheat("Radarhack & Bombtimer") == 1) {
							if ((int)team == (int)myteam)
								color = D3DCOLOR_ARGB(255, 0, 255, 0);
							else
								color = D3DCOLOR_ARGB(255, 255, 0, 0);
							DrawFilledRectangle(-fi * deltaX + 147 - 3, fi*deltaY + 147 - 3, -fi * deltaX + 147 + 3, fi*deltaY + 147 + 3, color);
						}

						rvm(PVOID(vmatrixptr + 0x5a0), 4, &flickerCheck);
						if (flickerCheck <= -1.0f)
							rvm(PVOID(vmatrixptr + 0x5B4), 64, &viewmatrix);

						//get3Ddist
						enemyDistance = sqrtss(deltaXold*deltaXold + deltaYold * deltaYold + deltaZ * deltaZ);

						entity = rpm(0x24000000 + 0x3CF20C + 8 * (i + 2));
						rvm(PVOID(entity + 0x138), 1, &bDormant);

						if ((int)team == (int)myteam)
							color = D3DCOLOR_ARGB(255, 0, 255, 0);
						else color = D3DCOLOR_ARGB(255, 255, 0, 0);

						if (!bDormant && WorldToScreen(viewmatrix, coords, &xl, &yl, &wl)) {
							if (cheat("ESP") == 1 || cheat("ESP") == 2)
							{
								DrawBorderBox(xl - 10000 / enemyDistance, yl - 10, 20000 / enemyDistance, 40000 / enemyDistance, 3, color);
								//drawHPhere
								DrawFilledRectangle(
									xl - 10000 / enemyDistance,
									yl - 18,
									xl - (10000 / enemyDistance) + (20000 / enemyDistance / 100 * hp) + 3,
									yl - 15,
									D3DCOLOR_XRGB(255, 255, 255));
							}

							if (cheat("ESP") == 2)
							{
								color = D3DCOLOR_XRGB(255, 255, 255);
								ID3DXFont* pFont;
								D3DXCreateFont(p_Device, 12, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);
								rvm(PVOID(ptr + offset + 0x8 + i * PLRSZ), 32, &espname);
								itoa(i + 2, espid, 10);
								DrawString((char*)espid, xl - 10000 / enemyDistance + 20000 / enemyDistance + 7, yl - 6 + 12 * 0, color, pFont);
								DrawString((char*)espname, xl - 10000 / enemyDistance + 20000 / enemyDistance + 21, yl - 6 + 12 * 0, color, pFont);
								itoa(hp, esphp, 10);
								rvm(PVOID(entity + 0xA4C + 0x5c4), 4, &armor);
								itoa(armor, esparm, 10);

								DrawString((char*)"H", xl - 10000 / enemyDistance + 20000 / enemyDistance + 7, yl - 6 + 12 * 1, color, pFont);
								DrawString((char*)esphp, xl - 10000 / enemyDistance + 20000 / enemyDistance + 14, yl - 6 + 12 * 1, color, pFont);
								DrawString((char*)"A", xl - 10000 / enemyDistance + 20000 / enemyDistance + 38, yl - 6 + 12 * 1, color, pFont);
								DrawString((char*)esparm, xl - 10000 / enemyDistance + 20000 / enemyDistance + 45, yl - 6 + 12 * 1, color, pFont);

								rvm(PVOID(engine_dll_base + 0x3958C8), 4, &steamidptr);
								rvm(PVOID(steamidptr + 0x38), 4, &steamidptr);

								if (steamidptr == 0)
								{
									steamidptr = rpm(engine_dll_base + 0x3958A8);
									steamidptr = rpm(steamidptr + 0x38);

									rvm(PVOID(steamidptr + 0x24), 4, &steamidptr);
									rvm(PVOID(steamidptr + 0x14 + (0x28 * (i + 1))), 4, &steamidptr);
									rvm(PVOID(steamidptr + 0x24), 20, &steamid);
								}
								else {
									rvm(PVOID(steamidptr + 0x24), 4, &steamidptr);
									rvm(PVOID(steamidptr + 0x14 + (0x28 * (i + 1))), 4, &steamidptr);
									rvm(PVOID(steamidptr + 0x24), 20, &steamid);
								}

								DrawString((char*)steamid, xl - 10000 / enemyDistance + 20000 / enemyDistance + 7, yl - 6 + 12 * 2, color, pFont);

								rvm(PVOID(entity + 0xA4C - 0xC + 0x5c4), 4, &money);
								itoa(money, espmon, 10);
								DrawString((char*)"$", xl - 10000 / enemyDistance + 20000 / enemyDistance + 7, yl - 6 + 12 * 3, color, pFont);
								DrawString((char*)espmon, xl - 10000 / enemyDistance + 20000 / enemyDistance + 13, yl - 6 + 12 * 3, color, pFont);

								rvm(PVOID(entity + 0x56C + 0x5c4), 1, &wepid);
								entity = rpm(0x24000000 + 0x3CF20C + 8 * wepid);
								entity = rpm(entity);
								entity = rpm(entity - 0x4);
								entity = rpm(entity - 0x8);

								rvm(PVOID(entity + 0xA + 4), 24, &espwep);

								for (int i = 0; i < 24; i++)
								{
									if (espwep[i] == 0x40)
										espwep[i] = 0x0;
								}


								DrawString((char*)espwep, xl - 10000 / enemyDistance + 20000 / enemyDistance + 7, yl - 6 + 12 * 4, color, pFont);

								pFont->Release();
							}
							//Calculate target that closest to the crosshair
							if (cheat("Aimbot").enabled > 0 && (int)team != (int)myteam)
							{
								hyp1 = sqrtss((xl - Width / 2) * (xl - Width / 2) + (yl - Height / 2) * (yl - Height / 2));
								hyp2 = sqrtss((xl_closest - Width / 2) * (xl_closest - Width / 2) + (yl_closest - Height / 2) * (yl_closest - Height / 2));

								if (hyp1 < hyp2 && hyp1 < aimfov)
								{
									xl_closest = xl;
									yl_closest = yl;
									closest = i;
								}
							}
						}
					}
				}

				if (xl_closest != 1000) {
					xl_closest_final = xl_closest;
					yl_closest_final = yl_closest;
					closest_final = closest;
				}
				else
				{
					xl_closest_final = 0 + Width / 2;
					yl_closest_final = 0 + Height / 2;
					closest_final = -2;
				}

				if (cheat("Radarhack & Bombtimer") == 1)
				{
					DrawFilledRectangle(144, 144, 151, 151, 100, 0, 255, 255); //white square on the center of the radar
					bombplanted = rpm(0x24000000 + 0x3FAB68); //we have a bomb?
					if (bombplanted) {
						if (xd == 0) {
#ifdef DEBUG
							cout << "Finding bomb ID..\n";
#endif
							maxentityid = rpm(0x24000000 + 0x3CF208);
							if (maxentityid > 0x1000) maxentityid = 0x1000;
							for (i = 64; i <= maxentityid; i++) {
								entity = rpm(0x24000000 + 0x3CF20C + 8 * i);
								entity = rpm(entity);
								entity = rpm(entity - 0x4);
								entity = rpm(entity - 0x8);

								espwep[0] = 0x0;
								rvm(PVOID(entity + 0x8), 24, &espwep);

								if (!strcmp((char*)espwep, (char*)".?AVC_PlantedC4@@"))
								{
#ifdef DEBUG
									cout << "We have a bomb (id " << hex << i << dec << ") ";
#endif
									CreateThread(0, 0, (LPTHREAD_START_ROUTINE)timer, 0, 0, 0);
									rvm(PVOID(0x24000000 + 0x3BF1D4 + 0x10 * i), 4, &boneptr);
									rvm(PVOID(boneptr + 0x4A8), 4, &boneptr);
									rvm(PVOID(boneptr + 0x0c), 4, &bombcoords[0]);
									rvm(PVOID(boneptr + 0x1c), 4, &bombcoords[1]);
									break;
								}
							}
						}
						ID3DXFont* pFont;
						D3DXCreateFont(p_Device, 20, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);
						DrawBorderBox(Width / 2 - 285, Height / 2 + 295, 565, 45, 5, D3DCOLOR_XRGB(7, 231, 171));
						DrawFilledRectangle(Width / 2 - 280, Height / 2 + 300, Width / 2 - 280 + xd, Height / 2 + 340, D3DCOLOR_ARGB(150, 10, 45, 41));
						DrawString((char*)(std::to_string(bomb).c_str()), Width / 2 - 5, Height / 2 + 310, 4, D3DCOLOR_XRGB(255, 255, 255), pFont);
						pFont->Release();

						deltaX = myposX - bombcoords[0];
						deltaY = myposY - bombcoords[1];

						deltaXold = deltaX; deltaYold = deltaY;

						deltaX = deltaXold * cos(myangY * PI / 180) + deltaYold * sin(myangY* PI / 180);
						deltaY = -deltaXold * sin(myangY* PI / 180) + deltaYold * cos(myangY* PI / 180);

						if (sqrtss(pow(deltaX, 2) + pow(deltaY, 2)) > 1500) {
							float k = 1500 / sqrtss(pow(deltaX, 2) + pow(deltaY, 2));
							deltaX = k * deltaX;
							deltaY = k * deltaY;
						}
						DrawFilledRectangle(-fi * deltaX + 147 - 3, fi*deltaY + 147 - 3, -fi * deltaX + 147 + 3, fi*deltaY + 147 + 3, D3DCOLOR_XRGB(200, 200, 200));
					}
				}

			} //esp,radar,aim enabled
		} //we on server?
}


void visnrec(bool d) {
	if (d)
	{
		wvm(LPVOID(0x24000000 + 0x192d2 + 0x2), sizeof(DWORD), 0);
		wvm(LPVOID(0x24000000 + 0x192dc + 0x2), sizeof(DWORD), 0);
#ifdef DEBUG
		cout << "visual norecoil enabled\n";
#endif
	}
	else
	{
		wvm(PVOID(0x24000000 + 0x192d2 + 0x2), 4, 0X00000bb0);
		wvm(PVOID(0x24000000 + 0x192dc + 0x2), 4, 0X00000BB4);
#ifdef DEBUG
		cout << "visual norecoil disabled\n";
#endif
	}
}

void Angleshack(bool d) {
	if (d)
	{
		angleshack = 1;

		SpyJmp(PVOID(0x2415D32E), freevisangX, 0);
		SpyJmp(PVOID(0x2415D3A0), freevisangY, 0);
		SpyJmp(PVOID(0x2415D412), freevisangZ, 0);
		SpyJmp(PVOID(0x240D46F9), fixPredict, 7);

		//cl_predictweapons 1 kill
		wpm(0x24000000 + 0x1e2858, 0x83068b9090909090);

#ifdef DEBUG
		cout << "angleshack enabled\n";
#endif
	}
	else
	{
		angleshack = 0;

		//freevisang revert
		wpm(0x2415D32E, 0xCE8B51106A0C4D8B);
		wpm(0x2415D3A0, 0xCE8B51106A104D8B);
		wpm(0x2415D412, 0xCE8B51086A144D8B);

		//predict revert
		wpm(0x240D46F9, 0x8B0C5089);
		wpm(0x240D46F9 + 4, 0x50891051);
		wpm(0x240D46F9 + 8, 0x14518b10);

		//set Z to 0 
		wpm(engine_dll_base + 0x39541c + 4 - enginedelta, 0);

		//cl_predictweapons 1 back
		wpm(0x24000000 + 0x1e2858, 0x83068bFFFF2803E8);
		Sleep(100);
		SendCMD("cl_predictweapons 1");
#ifdef DEBUG
		cout << "angleshack disabled\n";
#endif
	}
}

void TriggerCheck()
{
	float speed;

	svcheatsptr = rpm(0x24000000 + 0x3BEA44);
#ifdef DEBUG
	cout << "sv cheats pointer = 0x" << svcheatsptr << endl;
#endif
	timescaleptr = rpm(0x24000000 + 0x3E1D20);

	float boostang = 40.0f; 
	int sleeptim = 30;

	while (true)
	{
		if (tWnd == GetForegroundWindow() && cheat("No Recoil & Spread") == 2 && GetAsyncKeyState(VK_LBUTTON) < 0)
		{
			wpm(svcheatsptr + 0x314, 1);
			SendCMD("cl_predict 0");
			while (tWnd == GetForegroundWindow() && cheat("No Recoil & Spread") == 2 && GetAsyncKeyState(VK_LBUTTON) < 0)
				Sleep(10);
			SendCMD("cl_predict 1");
			Sleep(100);
			wpm(svcheatsptr + 0x314, 1);
		}

		if (tWnd == GetForegroundWindow() && cheat("Bunnyhop & Autostrafe") == 1 && GetAsyncKeyState(VK_SPACE) < 0)
		{

			while (GetAsyncKeyState(VK_SPACE) < 0)
			{
				if (cheat("Spinbot & AntiAim") != 1) {
					if (boostsleep)
					{
						Sleep(300);
						boostsleep = 0;
					}
					wpm(0x24000000 + 0x3E7250, 0); //-moveright
					wpm(0x24000000 + 0x3E725C, 1); //+moveleft
					wpm(engine_dll_base + 0x39541c + 4 - enginedelta, boostang); //set z ang
					Sleep(sleeptim);
					wpm(0x24000000 + 0x3E725C, 0); //-moveleft
					wpm(0x24000000 + 0x3E7250, 1); //+moveright
					wpm(engine_dll_base + 0x39541c + 4 - enginedelta, -boostang); //set z ang
					Sleep(sleeptim);

				}
				else
				{
					wpm(0x12330, 180.0f);
					Sleep(sleeptim);
				}

			}
			wpm(0x24000000 + 0x3E725C, 0); //-moveleft
			wpm(0x24000000 + 0x3E7250, 0); //-moveright
		}

		if (cheat("Speedhack").enabled != 10 && tWnd == GetForegroundWindow() && GetAsyncKeyState(0x12) < 0) //alt pressed
		{
			speed = (float)cheat("Speedhack").enabled / 10;
			wpm(svcheatsptr + 0x314, 1);
			wpm(timescaleptr + 0x108, speed);

			//waiting
			while (GetAsyncKeyState(0x12) < 0)
				Sleep(1);

			wpm(svcheatsptr + 0x314, 0);
			wpm(timescaleptr + 0x108, 1.0f);
		}

		if (tWnd == GetForegroundWindow() && GetAsyncKeyState(0x46) < 0) { //F
			if (!fullbright)
			{
				byte bytes[] = { 0x90, 0x90 };
				wvm(PVOID((DWORD)whlight), sizeof(bytes), bytes);
				fullbright = 1;
				SendCMD("r_decals 0");
			}
			else
			{
				byte bytes[] = { 0xEB, 0x08 };
				wvm(PVOID((DWORD)whlight), sizeof(bytes), bytes);
				fullbright = 0;
				SendCMD("r_decals 900");
			}
			Sleep(200);
		}

		if (cheat.Triggered("No Recoil & Spread"))
		{
			cheat.Update("No Recoil & Spread");

			if (cheat("No Recoil & Spread") != 0)
				visnrec(1);
			else
				visnrec(0);

			if (cheat("No Recoil & Spread") == 1)
			{
				if (!angleshack)
				{
					Angleshack(1);
					SendCMD("cl_predictweapons 0");
					Sleep(50);
				}
			}

			if (cheat("No Recoil & Spread") != 1)
			{
				if (cheat("Spinbot & AntiAim") == 0 && angleshack) {
					Angleshack(0);
					SendCMD("bind w +forward; bind s +back; bind d +moveright; bind a +moveleft; stm");
				}
			}
		}

		if (cheat.Triggered("Spinbot & AntiAim"))
		{
			cheat.Update("Spinbot & AntiAim");
			if (cheat("Spinbot & AntiAim").enabled > 0)
			{
				if (cheat("Spinbot & AntiAim") == 1)
					SendCMD("bind a +moveleft; bind w +moveleft; bind d +moveleft; bind s +moveright; stm; cl_predictweapons 0"); //spinbot
				if (cheat("Spinbot & AntiAim") == 2)
					SendCMD("bind w +forward; bind s +back; bind a +moveright; bind d +moveleft; stm; cl_predictweapons 0"); //fakeangles
				if (cheat("Spinbot & AntiAim") == 3)
					SendCMD("bind w +forward; bind s +back; bind a +moveleft; bind d +moveright; stm; cl_predictweapons 0"); //upsidedown
				if (cheat("Spinbot & AntiAim") == 4)
					SendCMD("bind w +back; bind s +forward; bind a +moveright; bind d +moveleft; stm; cl_predictweapons 0"); //backwards

				if (!angleshack)
					Angleshack(1);
				Sleep(50);
			}
			else
			{
				SendCMD("bind w +forward; bind s +back; bind a +moveleft; bind d +moveright; stm");
				if (cheat("No Recoil & Spread") != 1)
					Angleshack(0);
			}
		}

		if (cheat.Triggered("Play HLDJ"))
		{
			DWORD thisptr;
			rvm(PVOID(0x24000000 + 0x3EDD00), 4, &thisptr);
			if (cheat("Play HLDJ") == 1) {
				wpm(PVOID(aobconsole), 1, &nop);
				wpm(PVOID(aobconsole + 1), 1, &nop);
				wpm(thisptr + 0xD7C, 1);
				SendCMD("voice_loopback 1");
				Sleep(100);
				SendCMD("+voicerecord");
				byte bytes[] = { 0xF3, 0xA4 };
				wpm(PVOID(aobconsole), 2, &bytes);
			}
			else {
				wpm(thisptr + 0xD7C, 0x00000000);
				SendCMD("voice_loopback 0");
				SendCMD("-voicerecord");
			}
			cheat("Play HLDJ").trigger = cheat("Play HLDJ").enabled;
			cheat.Update("Play HLDJ");
		}

		if (cheat.Triggered("Radarhack & Bombtimer"))
		{
			cheat.Update("Radarhack & Bombtimer");

			if (cheat("Radarhack & Bombtimer") == 1)
				//radaralpha = 1
				wpm(0x24000000 + 0x3FF464, 1);
			else {
				//radaralpha = 255
				wpm(0x24000000 + 0x3FF464, 255);
			}

			if (cheat("Radarhack & Bombtimer") != 3)
			{
				wpm(spec1, 0x085E89);
				wpm(spec1 + 3, 0x90909090);

				wpm(0x24000000 + 0x257351, 0x5E5F1FD9);
				wpm(0x24000000 + 0x257351 + 4, 0x000CC2);

				radar3ptr = rpm(0x24000000 + 0x4035c0);
				wpm(radar3ptr + 0x8, 3); //radar type
				wpm(localplayer + 0xD80, 0); //spec type
			}

			if (cheat("Radarhack & Bombtimer") == 3)
			{
				wpm(spec1, 0x0846C7);
				wpm(spec1 + 3, 0x00000001);

				SpyJmp(LPVOID(0x24000000 + 0x257351), LPVOID(spec2), 2);

				radar3ptr = rpm(0x24000000 + 0x3E1A44);
				radar3ptr = rpm(radar3ptr + 0x4);
				wpm(radar3ptr + 0x5348, 0x243E1A44); //map size

				radar3ptr = rpm(0x24000000 + 0x4035c0);
				wpm(radar3ptr + 0x8, 1); //radar type
				wpm(localplayer + 0xD80, 5); //spec type
			}
		}

		if (cheat.Triggered("Visual Flyhack"))
		{
			cheat.Update("Visual Flyhack");
			if (cheat("Visual Flyhack") == 1)
			{
				byte dontmove[] = { 0x90, 0x90, 0x90 };
				wpm(PVOID(0x24000000 + 0xF657C), 3, &dontmove);

				SpyJmp(LPVOID(0x24000000 + 0xB841B), LPVOID(flycave), 9);
				wpm(0x24000000 + 0x3BE9D8, 0);

				//disable predict
				wpm(0x24000000 + 0x12DA64, 0x7C832C75);
				wpm(0x24000000 + 0x12bb9a, 0x4F8B909090909090);

			}
			else {
				byte move[] = { 0x89, 0x48, 0x08 };
				wpm(PVOID(0x24000000 + 0xF657C), 3, &move);

				byte flydisbytes[] = { 0x89, 0x11, 0x8B, 0x50, 0x08, 0x89, 0x51, 0x04, 0x8b, 0x40, 0x0c, 0x89, 0x41, 0x08, 0xc3 };
				wpm(LPVOID(0x24000000 + 0xB841B), sizeof(flydisbytes), &flydisbytes);
				wpm(0x24000000 + 0x3BE9D8, 1.0f);

				//enable predict
				wpm(0x24000000 + 0x12DA64, 0x7C832C74);
				wpm(0x24000000 + 0x12bb9a, 0x4F8B000002a48689);
			}
#ifdef DEBUG
			std::cout << "flyhack triggered\n";
#endif
		}

		if (cheat.Triggered("Smart Crosshair"))
		{
			cheat.Update("Smart Crosshair");
			if (cheat("Smart Crosshair") == 1)
				wpm(0x24000000 + 0x3E208C, 0);
			else wpm(0x24000000 + 0x3E208C, 1);
#ifdef DEBUG
			std::cout << "xhair triggered\n";
#endif
		}

		if (cheat.Triggered("No Smoke & Flash"))
		{
			noSmokeFlash();
			cheat.Update("No Smoke & Flash");
		}

		if (cheat.Triggered("No Hands & Sky"))
		{
			noHandsSky();
			cheat.Update("No Hands & Sky");
		}

		if (cheat.Triggered("Disable All & Exit"))
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DisExit, 0, 0, 0);


		if (cheat.Triggered("Chameleon Wallhack"))
		{
			WH();
			cheat.Update("Chameleon Wallhack");
		}

		if (cheat.Triggered("Bunnyhop & Autostrafe"))
		{
			if (cheat("Bunnyhop & Autostrafe") == 1)
			SpyInjectAndJump(OngroundToZero, PVOID(0x24000000 + 0x220FBB), 1);
			else
			{
				byte bytes[] = { 0x83, 0x41, 0x20, 0xff, 0x89, 0x10 };
				wvm(PVOID(0x24000000 + 0x220FBB), sizeof(bytes), bytes);
			}
			cheat.Update("Bunnyhop & Autostrafe");
		}

		Sleep(10);
	}
}
