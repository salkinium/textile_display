
iOS Controller App
==================

The app is designed to work on an (Retina) 3.5 inch display and features a simple
drawing mode and a scrolling text mode with a selection of four fonts.

It has three views:

- the display view shows the current display frame, connection status
	and allows for text input.
	You can clear the matrix or pause display refresh.
- the color picker lets you choose a color.
- the settings view lets you connect to an IP, select the text font and text
	scrolling verlocity.

The Color Picker was made by [Ryota Hayashi][color] and slightly modified.
The font and font renderer is taken from [XPCC][xpcc].
Data transmission is done via [AsyncSocket][socket].
The Font Picker is from [Tim Cinel][picker]

[color]: https://github.com/hayashi311/Color-Picker-for-iOS
[xpcc]: https://github.com/roboterclubaachen/xpcc
[socket]: https://github.com/robbiehanson/CocoaAsyncSocket
[picker]: https://github.com/TimCinel/ActionSheetPicker
