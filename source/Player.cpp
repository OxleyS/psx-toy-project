#include "Player.h"
#include "GameObject.h"
#include "GCRender.h"
#include "Mesh.h"
#include "Resources.h"
#include "Input.h"
#include "Camera.h"

Player::Player()
{
    const char* pMeshName = "YORDOOD.OXM";
    m_pGCRender = new GCRender;

	Mesh* pMesh = Resources::GetLoadedMesh(pMeshName);
	if (!pMesh) pMesh = Resources::BlockingMeshLoad(pMeshName);
    m_pGCRender->m_pMesh = pMesh;

    m_pFollowCamera = NULL;
    m_TargetForward = Vec3Short::Forward;
}

void Player::Update()
{
    if (m_pFollowCamera && !m_pFollowCamera->IsDebugMode())
    {
        ProcessInput();
        UpdateFollowCamera();
    }
}

void Player::ProcessInput()
{
    int xMove = 0, zMove = 0;
    static const int moveScalar = 40;

    if (Input::ButtonDown(Input::BUTTON_DLEFT, 0)) xMove -= moveScalar;
    if (Input::ButtonDown(Input::BUTTON_DRIGHT, 0)) xMove += moveScalar;
    if (Input::ButtonDown(Input::BUTTON_DUP, 0)) zMove += moveScalar;
    if (Input::ButtonDown(Input::BUTTON_DDOWN, 0)) zMove -= moveScalar;

    Matrix& playerMtx = m_pGCRender->m_WorldMtx;
    Matrix& cameraMtx = *m_pFollowCamera->GetCameraMatrix();
    Vec3Long offset;
    offset.vx = (cameraMtx.m[0][0] * xMove);
    offset.vy = (cameraMtx.m[0][1] * xMove);
    offset.vz = (cameraMtx.m[0][2] * xMove);
    offset.vx += (cameraMtx.m[2][0] * zMove);
    offset.vy += (cameraMtx.m[2][1] * zMove);
    offset.vz += (cameraMtx.m[2][2] * zMove);

    //m_TargetForward = offset;

    playerMtx.SetTrans(playerMtx.GetTrans() + (offset / GTE_ONE));
}

void Player::UpdateFollowCamera()
{
    const Matrix& playerMtx = m_pGCRender->m_WorldMtx;
    const Matrix& cameraMtx = *m_pFollowCamera->GetCameraMatrix();
    static const int followDistance = 3000;
    static const int followHeightOffset = -600;

    Vec3Long followPoint = playerMtx.GetTrans();
    followPoint.vy += followHeightOffset;

    Vec3Long cameraForward = Vec3Long::FromShort(cameraMtx.GetForward());
    Vec3Long desiredCameraPos = followPoint - ((cameraForward * followDistance) / GTE_ONE);
    m_pFollowCamera->m_Position = desiredCameraPos;
    m_pFollowCamera->m_bDirty = true;
}
