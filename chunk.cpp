#include "chunk.h"

Chunk::Chunk(unsigned int VAO, int worldx, int worldy) : VAO(VAO), worldx(worldx), worldy(worldy) {
	std::memset(blocks, 0, sizeof(blocks));
}

void Chunk::draw() {
	glBindVertexArray(VAO);
	constexpr int n = sizeof(cubeVertices) / sizeof(cubeVertices[0]);
	
	for (int x = 0; x < CHUNK_MAX_X; x++) {
		for (int y = 0; y < CHUNK_MAX_Y; y++) {
			for (int z = 0; z < CHUNK_MAX_Z; z++) {
				if (blocks[x][y][z] == BlockType::AIR) continue;
				
				float vertices[n];
				std::copy(cubeVertices, cubeVertices + n, vertices);
				
				// add offset to vertices
				for (int i = 0; i < n; i++) {
					if (i % 3 == 0) { // x coord
						vertices[i] += x;
					}
					else if (i % 3 == 1) { // y coord
						vertices[i] += y;
					}
					else { // z coord
						vertices[i] += z;
					}
				}

				// bind the data to the new vertices
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

				// draw block
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			}
		}
	}
	
	// reset the buffer data
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
}

