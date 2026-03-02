#include "c_xyz.h"
#include "util.h"
#include "d_bg_w.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

const cXyz wallB = { 9060.82617, 0, -2181.70850};
const cXyz wallA = {9320.65625, 0,  -1810.63269};
const cXyz wallN = {0.819152, 0,  -0.573577};

double cam_x = wallA.x;
double cam_z = wallA.z;

double zoom = 1.0;

class SeamViewerMain : public olc::PixelGameEngine
{
    
public:
SeamViewerMain()
	{
		sAppName = "SeamViewer";
	}

public:
	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float elapsed_time) override
	{
        int mwheel = GetMouseWheel();

        if (mwheel < 0) {
            zoom /= 2;
        }
        else if (mwheel > 0) {
            zoom *= 2;
        }

        if (GetKey(olc::UP).bHeld) {
            cam_z -= 250 * elapsed_time / zoom;
        }
        if (GetKey(olc::DOWN).bHeld) {
            cam_z += 250 * elapsed_time / zoom;
        }
        if (GetKey(olc::RIGHT).bHeld) {
            cam_x += 250 * elapsed_time / zoom;
        }
        if (GetKey(olc::LEFT).bHeld) {
            cam_x -= 250 * elapsed_time / zoom;
        }

        olc::vi2d mpos = GetMousePos();
        float mx = (double)(mpos.x-GetScreenSize().x/2) / zoom + cam_x;
        float mz = (double)(mpos.y-GetScreenSize().y/2) / zoom + cam_z;

        for (int x = -GetScreenSize().x/2; x < GetScreenSize().x/2; x++) {
            for (int z = -GetScreenSize().y/2; z < GetScreenSize().y/2; z++) {
                double xz = (double)x / (double)zoom + cam_x;
                double zz = (double)z / (double)zoom + cam_z;

                if (int result = wall_correct(
                    wallN, wallA, wallB, {(float)xz, 0.0f, (float)zz}
                )) {
                    DrawRect({x+GetScreenSize().x/2, z+GetScreenSize().y/2}, {1, 1}, result == 1 ? olc::DARK_CYAN : olc::VERY_DARK_CYAN);
                }
                else {
                    DrawRect({x+GetScreenSize().x/2, z+GetScreenSize().y/2}, {1, 1}, olc::DARK_GREY);
                }

                if (pointAboveLine({wallA.x, wallA.z}, {wallB.x, wallB.z}, {xz, zz})) {
                    DrawRect({x+GetScreenSize().x/2, z+GetScreenSize().y/2}, {1, 1}, olc::DARK_RED);
                }

                if ((float)xz == mx && (float)zz == mz) {
                    DrawRect({x+GetScreenSize().x/2, z+GetScreenSize().y/2}, {1, 1}, olc::GREEN);
                }
            }
        }

        DrawStringDecal({10, 10}, std::format("Zoom: {}x", zoom), olc::CYAN, {2, 2});
        DrawStringDecal({10, 30}, std::format("Camera: {:.6f},{:.6f}", cam_x, cam_z), olc::CYAN, {2, 2});
        DrawStringDecal({10, 50}, std::format("Hover: {:.6f},{:.6f}", mx, mz), olc::CYAN, {2, 2});

		return true;
	}
};

int main()
{
	SeamViewerMain sv_main;
	
    if (sv_main.Construct(512, 512, 1, 1, false, false, false, false)) {
        sv_main.Start();
    }

	return 0;
}
