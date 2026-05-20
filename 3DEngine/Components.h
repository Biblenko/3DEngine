#pragma once
#include "pch.h"

#include <vector>
#include <string>

#include "Mesh.h"
#include "Texture2D.h"
#include "Sprite.h"

#include "ResourceManager.h"

namespace Engine
{
    struct NameComponent 
    {
        std::string m_name;
        NameComponent(const std::string& name) : m_name(name) {}
    };

    struct TransformComponent 
    {
    public:
        glm::vec3 m_position;
        glm::vec3 m_rotation;
        glm::vec3 m_scale;


        mutable glm::mat4 m_cachedMatrix;
        mutable glm::mat3 m_cachedNormalMatrix;
        mutable bool m_isDirty;

        TransformComponent()
            : m_position(0.0f), m_rotation(0.0f), m_scale(1.0f), m_cachedMatrix(1.0f), m_cachedNormalMatrix(1.0f), m_isDirty(true) {
        }

        TransformComponent(float x, float y, float z)
            : m_position(x, y, z), m_rotation(0.0f), m_scale(1.0f), m_cachedMatrix(1.0f), m_cachedNormalMatrix(1.0f), m_isDirty(true) {
        }


        void SetPosition(const glm::vec3& pos) {
            m_position = pos;
            m_isDirty = true;
        }

        void SetRotation(const glm::vec3& rot) {
            m_rotation = rot;
            m_isDirty = true;
        }

        void SetScale(const glm::vec3& scl) {
            m_scale = scl;
            m_isDirty = true;
        }


    public:
        const glm::mat4& GetModelMatrix() const {
            if (m_isDirty) {
                UpdateMatrices();
            }
            return m_cachedMatrix;
        }

        const glm::mat3& GetNormalMatrix() const {
            if (m_isDirty) {
                UpdateMatrices();
            }
            return m_cachedNormalMatrix;
        }

        void UpdateMatrices() const {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, m_position);
            model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
            model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
            model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
            model = glm::scale(model, m_scale);

            m_cachedMatrix = model;

            // Нормал-матрица: транспонированная обратная от 3x3 части модельной матрицы
            m_cachedNormalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

            m_isDirty = false;
        }

        glm::vec3 GetForward() const {
            glm::vec3 front(0.f);

            float pitch = m_rotation.x;
            float yaw = m_rotation.y - 90.0f;

            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

            return glm::normalize(front);
        }

        glm::vec3 GetRight() const {
            glm::vec3 forward = GetForward();
            return glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
        }
    };

    struct CameraComponent
    {
        glm::mat4 m_projectionMatrix;

        float m_fov;
        float m_aspect;
        float m_zNear;
        float m_zFar;

        CameraComponent()
            : m_fov(45.0f), m_aspect(1.33f), m_zNear(0.1f), m_zFar(1000.0f)
        {
            UpdateProjection();
        }


        void SetAspect(float width, float height) {
            if (height <= 0) height = 1;
            m_aspect = width / height;
            UpdateProjection();
        }

        void UpdateProjection() {
            m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspect, m_zNear, m_zFar);
        }
    };

    struct ActiveCamera {};

    struct MeshComponent
    {
        std::shared_ptr<Mesh> m_Mesh;

        MeshComponent(std::shared_ptr<Mesh> mesh) 
        {
            m_Mesh = mesh;
        }

        void Render() const
        {
            if (m_Mesh) {
                m_Mesh->render();
            }
        }
    };

    struct SpriteComponent
    {
        std::shared_ptr<Sprite> m_sprite;

        SpriteComponent(std::shared_ptr<Sprite> _sprite)
        {
            m_sprite = _sprite;
        }

        void Render() const
        {
            if (m_sprite) {
                m_sprite->render();
            }
        }
    };

    struct MaterialComponent 
    {
        glm::vec3 m_ambient;
        glm::vec3 m_diffuse;
        glm::vec3 m_specular;
        float m_shininess;

        //GLuint m_diffuseMap = 0;

        std::string m_textureName;
        std::shared_ptr<Texture2D> m_diffuseMap;

        MaterialComponent()
            : m_ambient(0.1f), m_diffuse(1.0f), m_specular(0.5f), m_shininess(32.0f) {
        }


        MaterialComponent(glm::vec3 a, glm::vec3 d, glm::vec3 s, float sh)
            : m_ambient(a), m_diffuse(d), m_specular(s), m_shininess(sh) {
        }

        MaterialComponent(glm::vec3 a, glm::vec3 d, glm::vec3 s, float sh, std::shared_ptr<Texture2D> diffuseMapTexture)
            : m_ambient(a), m_diffuse(d), m_specular(s), m_shininess(sh) {
            m_diffuseMap = diffuseMapTexture;
        }

        MaterialComponent(glm::vec3 a, glm::vec3 d, glm::vec3 s, float sh, const std::string &_textureName)
            : m_ambient(a), m_diffuse(d), m_specular(s), m_shininess(sh) {
            m_textureName = _textureName;
            m_diffuseMap = ResourceManager::Instance->getTexture(m_textureName);
        }

        void setTexture(const std::string& _textureName)
        {
            m_textureName = _textureName;
            m_diffuseMap = ResourceManager::Instance->getTexture(m_textureName);
        }

        static MaterialComponent Gold() {
            return MaterialComponent(
                glm::vec3(0.24725f, 0.1995f, 0.0745f),    // Ambient
                glm::vec3(0.75164f, 0.60648f, 0.22648f),  // Diffuse
                glm::vec3(0.62828f, 0.5558f, 0.366065f),  // Specular
                51.2f
            );
        }

        static MaterialComponent RedPlastic() {
            return MaterialComponent(
                glm::vec3(0.3f, 0.1f, 0.0f),
                glm::vec3(0.5f, 0.0f, 0.0f),
                glm::vec3(0.7f, 0.6f, 0.6f),
                8.f,
                ResourceManager::Instance->getTexture("Cobble")
            );
        }
    };

    struct MovementComponent
    {
        glm::vec3 m_velocity;      // Реальна швидкість з інерцією
        float m_moveSpeed;         // Максимальна швидкість руху
        float m_smoothness;        // Коофіцієнт плавності (Lerp speed)
        float m_mouseSensitivity;  // Чутливість мыші

        // m_smoothness:
        // 50.0f = Швидкая остановка
        // 2.0f  = Плавна кино-камера

        MovementComponent(float speed = 5.0f, float smooth = 10.0f, float sensitivity = 0.1f)
            : m_velocity(0.0f), m_moveSpeed(speed), m_smoothness(smooth), m_mouseSensitivity(sensitivity)
        {

        }
    };

    struct PointLightComponent {
        glm::vec3 m_color;
        float m_intensity;

        PointLightComponent(glm::vec3 c = glm::vec3(1.0f), float i = 0.5f)
            : m_color(c), m_intensity(i) {
        }
    };

    struct TextComponent {
        std::string text;
        std::shared_ptr<Texture2D> fontAtlas;
        std::shared_ptr<Mesh> textMesh;
        glm::vec3 color = glm::vec3(1.0f); // Цвет текста

        // Конструктор сразу генерирует меш
        TextComponent(const std::string& t, std::shared_ptr<Texture2D> atlas)
            : text(t), fontAtlas(atlas)
        {
            textMesh = MeshGenerator::Text(fontAtlas, text, -0.25f);
        }

        // Если текст поменялся (например, счетчик FPS), нужно перегенерировать меш
        void SetText(const std::string& newText) {
            if (text != newText) {
                text = newText;
                textMesh = MeshGenerator::Text(fontAtlas, text, -0.25f);
            }
        }
    };

    struct DataBaseConnectComponent {

    };
}