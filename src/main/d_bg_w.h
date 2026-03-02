#pragma once

#include "types.h"
#include "util.h"
#include "c_xyz.h"
#include "const.h"

bool positionWallCorrect(cXyz playerPos, cXyz wallNormal, f32 rNormalXZLen, f32 colliderDistanceToWallPusher) {
    f32 speed_correct = colliderDistanceToWallPusher - 1.0f;
    if (speed_correct < 0.0f) {
        speed_correct = 0.0f;
    }

    f32 move = speed_correct * rNormalXZLen;
    return ((playerPos.x + move * wallNormal.x) != playerPos.x) || ((playerPos.z + move * wallNormal.z) != playerPos.z);
}

int wall_correct(cXyz wallNormal, cXyz pointA, cXyz pointB, cXyz playerPos) {
    int corrected = false;

    f32 normalXZLen = JMAFastSqrt(wallNormal.x * wallNormal.x + wallNormal.z * wallNormal.z);

    f32 rNormalXZLen = 1.0f/normalXZLen;

    // Calculate the length of the wall pusher
    f32 wallPusherLength = rNormalXZLen * WALL_R;

    // Calculate the offset of the wall pusher from the wall itself
    Vec wallPusherOffset;
    wallPusherOffset.x = wallPusherLength * wallNormal.x;
    wallPusherOffset.y = 0.0f;
    wallPusherOffset.z = wallPusherLength * wallNormal.z;

    f32 wallSegmentX0 = pointA.x;
    f32 wallSegmentY0 = pointA.z;
    f32 wallSegmentX1 = pointB.x;
    f32 wallSegmentY1 = pointB.z;


    // Offset the wall pusher from the wall, normal to the wall, by the length of the circle radius
    wallSegmentX0 += wallPusherOffset.x;
    wallSegmentY0 += wallPusherOffset.z;
    wallSegmentX1 += wallPusherOffset.x;
    wallSegmentY1 += wallPusherOffset.z;

    // Calculate the nearest point on the wall pusher segment to the circle center
    f32 colliderDistanceToWallPusherSquared, wallPusherNearestPointX, wallPusherNearestPointZ;
    bool colliderPerpendicularToWall = cM3d_Len2dSqPntAndSegLine(
        playerPos.x, playerPos.z, wallSegmentX0, wallSegmentY0, wallSegmentX1, wallSegmentY1,
        &wallPusherNearestPointX, &wallPusherNearestPointZ, &colliderDistanceToWallPusherSquared
    );

    // Calculate the XZ offset to that nearest point from the circle center
    f32 circleOffsetFromWallPusherX = wallPusherNearestPointX - playerPos.x;
    f32 circleOffsetFromWallPusherZ = wallPusherNearestPointZ - playerPos.z;
    
    f32 circleRadiusSquared = WALL_RR;

    if (
        // Checks that the collider isn't too far from the wall pusher segment to collide
        !(colliderDistanceToWallPusherSquared > circleRadiusSquared) &&
        // Checks that the collider isn't on the other side of the wall
        !(circleOffsetFromWallPusherX * wallPusherOffset.x + circleOffsetFromWallPusherZ * wallPusherOffset.z < 0.0f)
    ) {
        if (colliderPerpendicularToWall) {
            if (positionWallCorrect(playerPos, wallNormal, rNormalXZLen, JMAFastSqrt(colliderDistanceToWallPusherSquared))) {
                corrected = 1;
            }
            else {
                corrected = 2;
            }
        } else {
            // The collider is along one of the ends of the segment, the corners of the wall
            wallSegmentX0 -= wallPusherOffset.x;
            wallSegmentY0 -= wallPusherOffset.z;
            wallSegmentX1 -= wallPusherOffset.x;
            wallSegmentY1 -= wallPusherOffset.z;
            // Find the collider distance to each corner so we can see which one we are closer to
            f32 circleDistanceToPusherSegmentStartSquared = cM3d_Len2dSq(
                wallSegmentX0, wallSegmentY0, playerPos.x, playerPos.z
            );

            f32 circleDistanceToPusherSegmentEndSquared = cM3d_Len2dSq(
                wallSegmentX1, wallSegmentY1, playerPos.x, playerPos.z
            );

            // Calculate negative normals of the wall
            f32 negWallNormalX = -wallNormal.x;
            f32 negWallNormalY = -wallNormal.z;

            if (circleDistanceToPusherSegmentStartSquared < circleDistanceToPusherSegmentEndSquared) {
                // We are closer to the start of the segment, correct based on that
                if (!(circleDistanceToPusherSegmentStartSquared > circleRadiusSquared) && !(fabsf(circleDistanceToPusherSegmentStartSquared - circleRadiusSquared) < 0.008f)) {
                    corrected = 1;
                }
            } else if (!(circleDistanceToPusherSegmentEndSquared > circleRadiusSquared) && !(fabsf(circleDistanceToPusherSegmentEndSquared - circleRadiusSquared) < 0.008f)) {
                corrected = 1;
            }
        }
    }

    return corrected;
}

