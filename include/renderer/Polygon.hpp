#ifndef _POLYGON__HPP_
#define _POLYGON__HPP_

class Polygon
{
protected:
	const uint64_t _numVertices;
	float*         _vertices;

public:
	// Constructor
	Polygon(const uint64_t numVertices) : _numVertices(numVertices), _vertices(new float[_numVertices * 3]) {}
	// Destructor
	~Polygon(void) { delete _vertices; }

	/* GETTERS */
	// Returns vertices pointer
	float* getVertices(void) { return _vertices; }

	/* SETTERS */
	// Sets the vertices
	void setVertices(float* vertices) { _vertices = vertices; }
};

#endif