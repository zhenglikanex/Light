#pragma once 
namespace meta_generated {
extern void Registercomponents();
extern void Registerscene_camera();
static void RegisterTypesGeneratedImpl() { 
Registercomponents();
Registerscene_camera();
}
}