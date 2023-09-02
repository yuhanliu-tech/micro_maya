#version 150
// ^ Change this to version 130 if you have compatibility issues

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself
uniform mat4 u_transform[100];

uniform mat4 u_bind[100];

in vec4 vs_Pos, vs_Col;

in vec2 vs_Weights;

in ivec2 vs_JointIDs;

out vec4 fs_Col;

void main() {

    fs_Col = vs_Col;                         // Pass the vertex colors to the fragment shader for interpolation

    vec4 modelposition = u_Model *
            ( vs_Weights.x * (u_transform[vs_JointIDs.x] * u_bind[vs_JointIDs.x] * vs_Pos) +
              vs_Weights.y * (u_transform[vs_JointIDs.y] * u_bind[vs_JointIDs.y] * vs_Pos) );

    gl_Position = u_ViewProj * modelposition;

}

