# asteroid
CPU-only 2D asteroid game in C
<div align="center">
<img width="922" height="759" alt="image" src="https://github.com/user-attachments/assets/4b906600-b40e-47ba-b659-a7d3ec74acd6" />
</div>

## General Info
This game is the second episode of the experiment "Let's Make a CPU-only game engine in C", in which I try to develop both a game and the engine itself from features that I would probably reuse for other games. You can check the first episode <a href="https://github.com/tdecorso/pong">here</a>, where I developed a simple 2D pong game.

## Engine new features
The engine grew substantially from the first episode. It now supports a proper rendering pipeline that you can also adapt to render 3D objects. I limited myself to the classic triangle rasterization through barycentric coordinates interpolation. This lead to sprites! So now you can render your own textures and move them. Keep in mind that in this engine you apply transformations in world-space (the origin is the center of the screen). 

The engine also supports a 2D camera (position and zoom), ortographic projection, and audio (through miniaudio for now).

## Limitations
I noticed that if you increment the number of sprites in the game, performance is quite limited. It was expected, since the engine still does not have any efficient way of managing entities. This will come in the future and I will try to understand what is the limit once you optimize entities management. I also didn't try to compile the project on Windows, but I don't expect any issue since the engine abstracts the platform using SDL.

## Dependencies
This project currently depends on libm for math, stb_image for texture loading, SDL3 for platform (window + user input) and miniaudio for audio.

## Credits

The 2D graphics come from the work of <a href="https://axassets.itch.io/">AX Assets</a>.

The sounds come from <a href="https://pixabay.com/">pixabay</a>.
