# software-engine

# Dependincies
glad
GLFW3
GLFW3native
GLM
STB
FREETYPE

# TO-DO
Tomorrow:
	finish currentCamera for scenes
Short-Term:
	1. make a set menu function instead of it being in the constructor
	2. Finish implementing menus:
		d. add tab icons
		z. duplicate all for mac and linux
	4. make menu func map[0] only default in window class
	6. Put mutex lock on accessing scene data.
	8. virtual destructors
	9. make addComponent templated
	10. Use shader component to allow for use of non scene shader
	11. Classes that use glad need to check that its initialized
	12. Error class
	13. make texcture class hold texture int instead of offset
	14. make a default shader for text

Long-Term:
	15. clean code check
	16. make perspective matrix in renderer use screen size instead of constant
	21. check virtuals for components.
	24. make moodification functions of scenes and objects threade safe
	26. inline functions

# NOTES 
tab separators can be added to the top level menu but are not displayed