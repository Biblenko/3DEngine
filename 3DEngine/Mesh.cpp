#include "pch.h"
#include "Mesh.h"

#include <GL/glew.h>

namespace Engine
{
	/// Конструктор мешу. Створює VAO, VBO та EBO, завантажує геометричні дані 
	/// у відеопам'ять (VRAM) та налаштовує розмітку вершинних атрибутів.
	Mesh::Mesh(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices)
		: m_indexCount(static_cast<unsigned int>(indices.size()))
	{
		// Створення та прив'язка Vertex Array Object (VAO), який зберігає стан атрибутів
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		// Створення, прив'язка та заповнення Vertex Buffer Object (VBO) сирими даними вершин
		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

		// Створення, прив'язка та заповнення Element Buffer Object (EBO) індексами для Indexed Drawing
		glGenBuffers(1, &m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

		// Загальний крок (stride) для інтерлівінг-масиву (перемішані дані): 
		// 8 float на одну вершину (3 координати позиції + 3 координати нормалі + 2 текстурні координати)
		GLsizei stride = 8 * sizeof(GLfloat);

		// Атрибут 0: Координати позиції (X, Y, Z) — 3 float, зміщення (offset) 0
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

		// Атрибут 1: Вектор нормалі (Nx, Ny, Nz) — 3 float, зміщення 3 float (пропускаємо позицію)
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));

		// Атрибут 2: Текстурні координати (U, V) — 2 float, зміщення 6 float (пропускаємо позицію та нормаль)
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));

		// Відв'язуємо VAO, щоб випадково не перезаписати його конфігурацію в інших частинах коду
		glBindVertexArray(0);
	}

	/// Конструктор переміщення (Move constructor). Передає володіння OpenGL-хендлами
	/// новому об'єкту, занулюючи старий, щоб уникнути подвійного видалення ресурсів у деструкторі.
	Mesh::Mesh(Mesh&& other) noexcept
		: m_VAO(other.m_VAO), m_VBO(other.m_VBO), m_EBO(other.m_EBO), m_indexCount(other.m_indexCount)
	{
		other.m_VAO = 0;
		other.m_VBO = 0;
		other.m_EBO = 0;
		other.m_indexCount = 0;
	}

	/// Оператор присвоювання переміщення (Move assignment operator).
	/// Спочатку очищає поточні OpenGL-ресурси об'єкта, а потім забирає хендли у `other`.
	Mesh& Mesh::operator=(Mesh&& other) noexcept
	{
		if (this != &other)
		{
			// Видаляємо старі буфери поточного об'єкта перед копіюванням нових
			glDeleteVertexArrays(1, &m_VAO);
			glDeleteBuffers(1, &m_VBO);
			glDeleteBuffers(1, &m_EBO);

			// Перехоплюємо хендли
			m_VAO = other.m_VAO;
			m_VBO = other.m_VBO;
			m_EBO = other.m_EBO;
			m_indexCount = other.m_indexCount;

			// Очищаємо тимчасовий об'єкт
			other.m_VAO = 0;
			other.m_VBO = 0;
			other.m_EBO = 0;
			other.m_indexCount = 0;
		}
		return *this;
	}

	/// Деструктор. Безпечно видаляє масив вершин та GPU-буфери з відеопам'яті.
	Mesh::~Mesh()
	{
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
		glDeleteBuffers(1, &m_EBO);
	}

	/// Виконує отрисовку геометрії мешу через індекси (Indexed Drawing).
	void Mesh::render() const
	{
		glBindVertexArray(m_VAO);

		// Рендеринг трикутників на основі збереженого індексного буфера EBO
		glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);

		// glBindVertexArray(0);
	}
}