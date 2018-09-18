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
    if (m_pFollowCamera && !Debug::IsDebugModeEnabled())
    {
        ProcessInput();
        UpdateFollowCamera();
    }

    UpdateFaceTarget();

    TestCollision();
}

void Player::ProcessInput()
{
    int xMove = 0, zMove = 0;
    static const int oneDirMoveScalar = 520;
    static const int twoDirMoveScalar = 368;

    if (Input::ButtonDown(Input::BUTTON_DLEFT, 0)) xMove -= 1;
    if (Input::ButtonDown(Input::BUTTON_DRIGHT, 0)) xMove += 1;
    if (Input::ButtonDown(Input::BUTTON_DUP, 0)) zMove += 1;
    if (Input::ButtonDown(Input::BUTTON_DDOWN, 0)) zMove -= 1;

    Matrix& playerMtx = m_pGCRender->m_WorldMtx;
    Matrix& cameraMtx = *m_pFollowCamera->GetCameraMatrix();

    Vec3Long moveRight = Vec3Long(cameraMtx.m[0][0], 0, cameraMtx.m[0][2]).GetNormalized();
    Vec3Long moveForward = Vec3Long(cameraMtx.m[2][0], 0, cameraMtx.m[2][2]).GetNormalized();

    if (xMove != 0 || zMove != 0)
    {
        int moveScalar = xMove != 0 && zMove != 0 ? twoDirMoveScalar : oneDirMoveScalar;
        Vec3Long offset = (moveRight * xMove) + (moveForward * zMove);
        m_TargetForward = Vec3Short::FromLong(offset.GetNormalized());

        playerMtx.SetTrans(playerMtx.GetTrans() + ((offset * moveScalar) / GTE_ONE));
    }
}

void Player::UpdateFollowCamera()
{
    const Matrix& playerMtx = m_pGCRender->m_WorldMtx;
    const Matrix& cameraMtx = *m_pFollowCamera->GetCameraMatrix();
    static const int followDistance = 10 * WORLD_UNIT;
    static const int followHeightOffset = -2 * WORLD_UNIT;

    Vec3Long followPoint = playerMtx.GetTrans();
    followPoint.vy += followHeightOffset;

    Vec3Long cameraForward = Vec3Long(cameraMtx.m[2][0], cameraMtx.m[2][1], cameraMtx.m[2][2]);
    Vec3Long desiredCameraPos = followPoint - ((cameraForward * followDistance) / GTE_ONE);
    m_pFollowCamera->m_Position = desiredCameraPos;
    m_pFollowCamera->m_bDirty = true;
}

void Player::UpdateFaceTarget()
{
    static const int distancePerFrame = 1000;

    Vec3Short curForward = m_pGCRender->m_WorldMtx.GetForward();
    Vec3Short difference;
    difference = m_TargetForward - curForward;

    if (difference.LengthSq() < distancePerFrame * distancePerFrame)
        m_pGCRender->m_WorldMtx.SetForward(m_TargetForward);
    else 
    {
        // Degenerate case?
        if (m_TargetForward.Dot(curForward) < -16700000)
        {
            // Fudge the vector to a direction to rotate
            curForward += Vec3Short(10, 10, 10);
        }
        LoadAverageShort12(&curForward, &m_TargetForward, 4096 - distancePerFrame, distancePerFrame, &curForward);
    }

    // Set the new look vector
    curForward.Normalize();
    m_pGCRender->m_WorldMtx.SetForward(curForward);

    // Cross product to get the other two vectors
    // TODO: MatrixNormal()?
    Vec3Long longForward = Vec3Long::FromShort(curForward);
    Vec3Long right = Vec3Long(0, GTE_ONE, 0).CrossFixed(longForward);
    right.Normalize();
    Vec3Long up = longForward.CrossFixed(right);
    m_pGCRender->m_WorldMtx.SetRight(Vec3Short::FromLong(right));
    m_pGCRender->m_WorldMtx.SetUp(up.GetNormalShort());
}

void Player::TestCollision()
{
    
}
