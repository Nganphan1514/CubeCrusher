// ma_nguon/TrangThaiGame.h
#pragma once

enum TrangThai
{
    MENU,
    CHON_MAY_BAY,
    CHON_VAT_CAN,
    CHOI_GAME
};

extern TrangThai trangThaiHienTai;

// Lưu lựa chọn
extern int selectedPlaneIndex;
extern int selectedObstacleIndex;
