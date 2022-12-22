#include "Component.h"
#include "GameObject.h"

Component::Component() { gameObject = nullptr; }

Component::Component(const Component& copy) { gameObject = nullptr; }

Component::~Component() { gameObject = nullptr; }

void Component::operator=(const Component& copy) { gameObject = nullptr; }
