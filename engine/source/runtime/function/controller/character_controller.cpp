#include "runtime/function/controller/character_controller.h"

#include "runtime/core/base/macro.h"

#include "runtime/function/framework/component/motor/motor_component.h"
#include "runtime/function/framework/world/world_manager.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/physics/physics_scene.h"

namespace Piccolo
{
    CharacterController::CharacterController(const Capsule& capsule) : m_capsule(capsule)
    {
        m_rigidbody_shape                                    = RigidBodyShape();
        m_rigidbody_shape.m_geometry                         = PICCOLO_REFLECTION_NEW(Capsule);
        *static_cast<Capsule*>(m_rigidbody_shape.m_geometry) = m_capsule;

        m_rigidbody_shape.m_type = RigidBodyShapeType::capsule;

        Quaternion orientation;
        orientation.fromAngleAxis(Radian(Degree(90.f)), Vector3::UNIT_X);

        m_rigidbody_shape.m_local_transform =
            Transform(Vector3(0, 0, capsule.m_half_height + capsule.m_radius), orientation, Vector3::UNIT_SCALE);
    }
/*
    Vector3 CharacterController::move(const Vector3& current_position, const Vector3& displacement)
    {
        std::shared_ptr<PhysicsScene> physics_scene =
            g_runtime_global_context.m_world_manager->getCurrentActivePhysicsScene().lock();
        ASSERT(physics_scene);

        Vector3 final_position = current_position + displacement;

        Transform final_transform = Transform(final_position, Quaternion::IDENTITY, Vector3::UNIT_SCALE);

        if (physics_scene->isOverlap(m_rigidbody_shape, final_transform.getMatrix()))
        {
            final_position = current_position;
        }

        return final_position;
    }
*/
    Vector3 CharacterController::move(const Vector3& current_position, const Vector3& displacement)
    {
        std::shared_ptr<PhysicsScene> physics_scene =
            g_runtime_global_context.m_world_manager->getCurrentActivePhysicsScene().lock();
        ASSERT(physics_scene);

        std::vector<PhysicsHitInfo> hits;
        std::vector<PhysicsHitInfo> ray_hits;

        Transform world_transform = Transform(
            current_position + 0.1f * Vector3::UNIT_Z,
            Quaternion::IDENTITY,
            Vector3::UNIT_SCALE);

        Vector3 vertical_displacement   = displacement.z * Vector3::UNIT_Z;
        Vector3 horizontal_displacement = Vector3(displacement.x, displacement.y, 0.f);

        Vector3 vertical_direction   = vertical_displacement.normalisedCopy();
        Vector3 horizontal_direction = horizontal_displacement.normalisedCopy();

        Vector3 final_position = current_position;

        hits.clear();
    
        world_transform.m_position -= 0.1f * Vector3::UNIT_Z;

        // vertical pass
        if (physics_scene->sweep(m_rigidbody_shape,
                                 world_transform.getMatrix(),
                                 vertical_direction,
                                 vertical_displacement.length(),
                                 hits))
        {
            final_position += hits[0].hit_distance * vertical_direction;
        }
        else
        {
            final_position += vertical_displacement;
        }

        hits.clear();

        // side pass
        // my code
        if (physics_scene->sweep(
            m_rigidbody_shape,
            world_transform.getMatrix(), 
            horizontal_direction, 
            horizontal_displacement.length(), 
            hits))
        {
            // final_position += hits[0].hit_distance * horizontal_direction;
            // when hit, final vector is project vector of horizontal_displacement at wall plane 
            Vector3 hit_n = hits[0].hit_normal;
            final_position += horizontal_displacement - (hit_n.dotProduct(horizontal_displacement)/hit_n.length())*hit_n.normalisedCopy();
        }
        else
        {
            final_position += horizontal_displacement;
        }

        return final_position;
    }
} // namespace Piccolo
