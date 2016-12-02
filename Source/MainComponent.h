
#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

/**
    A struct that handles the setup and layout of the DrumPadGridProgram
*/
int buttonGrid[5][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};
int buttonGrid2[5][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};
int blockUIDs[2] = {0,0};
struct SynthGrid
{
    SynthGrid (int cols, int rows)
        : numColumns (cols),
          numRows (rows)
    {
        constructGridFillArray(buttonGrid);
    }

	Colour getColorFromIndex(int colorIndex) 
	{
		Colour result = Colours::black;

		switch(colorIndex)
		{
			case 0:
				result = backgroundGridColour;
				break;
			case 1:
				result = Colours::red;
				break;
			case 2:
				result = Colours::orange;
				break;
			case 3:
				result = Colours::yellow;
				break;
			case 4:
				result = Colours::green;
				break;
			case 5:
				result = Colours::cyan;
				break;
			case 6:
				result = Colours::blue;
				break;
			case 7:
				result = Colours::purple;
				break;
			case 8:
				result = Colours::white;
				break;
			default:
				result = Colours::black;
		}

		return result;
	}
    /** Creates a GridFill object for each pad in the grid and sets its colour
        and fill before adding it to an array of GridFill objects
     */
    void constructGridFillArray(int buttonGrid[][5])
    {
        gridFillArray.clear();

        for (int i = 0; i < numRows; ++i)
        {
            for (int j = 0; j < numColumns; ++j)
            {
                DrumPadGridProgram::GridFill fill;
				int colorIndex = buttonGrid[i%5][j%5];
				fill.colour = getColorFromIndex(colorIndex);


				if(colorIndex == 0) {
					fill.fillType = DrumPadGridProgram::GridFill::FillType::pizzaHollow;
				}
				else {
					fill.fillType = DrumPadGridProgram::GridFill::FillType::gradient;
				}
                gridFillArray.add(fill);
            }
        }
    }

    int getNoteNumberForPad (int x, int y)
    {
        int xIndex = x / 3;
        int yIndex = y / 3;

        return 60 + ((4 - yIndex) * 5) + xIndex;
    }

    //==============================================================================
    int numColumns, numRows;
    float width, height;

    Array<DrumPadGridProgram::GridFill> gridFillArray;
    Colour touchColour = Colours::white;
    Colour baseGridColour = Colours::cyan;
    Colour backgroundGridColour = Colours::black;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthGrid)
};

/**
    The main component
*/
class MainComponent   : public Component,
						private OSCReceiver,
						private OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>,
                        public TopologySource::Listener,
                        private TouchSurface::Listener,
                        private ControlButton::Listener,
                        private Timer
{
public:
    MainComponent() : layout(5, 5)
    {
        setSize(350, 190);

        // Register MainContentComponent as a listener to the PhysicalTopologySource object
        topologySource.addListener(this);

        // specify here where to send OSC messages to: host URL and UDP port number
        if (! sender.connect ("127.0.0.1", 57120))
            showConnectionErrorMessage("Error: could not connect to UDP port 57120.");

        if (! connect(57140))
            showConnectionErrorMessage ("Error: could not connect to UDP port 57140.");

        // tell the component to listen for OSC messages matching this address:
        addListener(this, "/block/lightpad/0/setButton");
        addListener(this, "/block/lightpad/1/setButton");
    };

    ~MainComponent()
    {
        if (activeBlock != nullptr)
            detachActiveBlock();
    }

    void oscMessageReceived (const OSCMessage& message) override
    {

		if(message.getAddressPattern().toString() == "/block/lightpad/1/setButton") {
			// Only do something if there are 3 messages coming in
			if (message.size() == 3 && message[0].isInt32()) {
				int x = message[0].getInt32();
				int y = message[1].getInt32();
				int color = message[2].getInt32();

				buttonGrid2[x][y] = color;
				layout.constructGridFillArray(buttonGrid2);
				(gridPrograms[1])->setGridFills(layout.numColumns, layout.numRows, layout.gridFillArray);
			}
		}
		else if(message.getAddressPattern().toString() == "/block/lightpad/0/setButton") {
			// Only do something if there are 3 messages coming in
			if (message.size() == 3 && message[0].isInt32()) {
				int x = message[0].getInt32();
				int y = message[1].getInt32();
				int color = message[2].getInt32();

				buttonGrid[x][y] = color;
				layout.constructGridFillArray(buttonGrid);
				(gridPrograms[0])->setGridFills(layout.numColumns, layout.numRows, layout.gridFillArray);
			}
		}
    }
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::grey);
        g.drawText("Sending OSC data to 127.0.0.1 on port 57120:", 10, 10, 380, 20, true);
        g.drawText("/block/lightpad/0/on - (fingerIndex, x, y, z)", 10, 30, 380, 20, true);
        g.drawText("/block/lightpad/0/off - (fingerIndex)", 10, 45, 380, 20, true);
        g.drawText("/block/lightpad/0/position - (fingerIndex, x, y, z)", 10, 60, 380, 20, true);
        g.drawText("/block/lightpad/0/button - (1)", 10, 75, 380, 20, true);

        g.drawText("Receiving OSC data on port 57140:", 10, 95, 380, 20, true);
        g.drawText("You can create buttons (or set them black) by sending", 10, 110, 480, 20, true);
        g.drawText("to port 57140. There is a 5x5 grid of button possibilities.", 10, 125, 480, 20, true);
        g.drawText("/block/lightpad/0/setButton - (x, y, color)", 10, 140, 480, 20, true);
        g.drawText("/block/lightpad/1/setButton - (x, y, color)", 10, 155, 480, 20, true);
    }

    void resized() override {}

    /** Overridden from TopologySource::Listener, called when the topology changes */
    void topologyChanged() override
    {
        // Reset the activeBlock object
        if (activeBlock != nullptr)
            detachActiveBlock();

        // Get the array of currently connected Block objects from the PhysicalTopologySource
        Block::Array blocks = topologySource.getCurrentTopology().blocks;

		int blockIndex = 0;
        // Iterate over the array of Block objects
        for (auto b : blocks)
        {
            // Find the first Lightpad
            if (b->getType() == Block::Type::lightPadBlock)
            {
                activeBlock = b;

                // Register MainContentComponent as a listener to the touch surface
                if (auto surface = activeBlock->getTouchSurface())
                    surface->addListener (this);

                // Register MainContentComponent as a listener to any buttons
                for (auto button : activeBlock->getButtons())
                    button->addListener (this);

                // Get the LEDGrid object from the Lightpad and set its program to the program for the current mode
                if (auto grid = activeBlock->getLEDGrid())
                {
                    setLEDProgram(blockIndex, grid);
                }

                //break;
				// NEED A SAFETY HERE
				if(blockIndex < 2) {
					blockUIDs[blockIndex] = b->uid;
					std::cout << "found block" << blockIndex << std::endl;
					blockIndex++;

					//break;
				}
            }
        }
    }

private:
    OSCSender sender;
    /** Overridden from TouchSurface::Listener. Called when a Touch is received on the Lightpad */
    void touchChanged (TouchSurface& surface, const TouchSurface::Touch& touch) override
    {

		int blockIndexInt = 0;

		///Get the index of the UID (refactor into a function)
		for(int i=0; i < 2; i++) {
			if(blockUIDs[i] == (int)surface.block.uid) {
				blockIndexInt = i;
			}
		}

		std::string blockIndex = std::to_string(blockIndexInt);

        if (currentMode == waveformSelectionMode && touch.isTouchStart)
        {
            // Change the displayed waveshape to the next one
            ++waveshapeMode;

            if (waveshapeMode > 3)
                waveshapeMode = 0;
        }
        else if (currentMode == playMode)
        {
			float touchIndex = (float) touch.index,
				  startX = (float) touch.startX,
				  startY = (float) touch.startY,
				  x = (float) touch.x,
				  y = (float) touch.y,
				  z = (float) touch.z;
            // Limit the number of touches per second
            constexpr int maxNumTouchMessagesPerSecond = 100;
            auto now = Time::getCurrentTime();
            clearOldTouchTimes (now);

            // Send the touch event to the DrumPadGridProgram and Audio class
            if (touch.isTouchStart)
            {
				int padX=0,
					padY=0;

				padX = (int)(touch.startX/0.4);
				padY = (int)(touch.startY/0.4);

				// TODO: THIS IS WEIRD. Why are the x,y values inverting. INVESTIGATE
				if(buttonGrid[padY][padX] > 0) {
					gridPrograms[blockIndexInt]->startTouch(touch.startX, touch.startY);
				}

				const std::string patternString = "/block/lightpad/" + blockIndex + "/on";
				OSCAddressPattern pattern = OSCAddressPattern(patternString);
				if (! sender.send (pattern, touchIndex, startX/2, startY/2, z))
					showConnectionErrorMessage ("Error: could not send OSC message.");

				//bitmapProgram->setLED(roundToInt((touch.startX/2) * 16), roundToInt((touch.startY/2) * 16), Colours::purple);
            }
            else if (touch.isTouchEnd)
            {
                (gridPrograms[blockIndexInt])->endTouch(touch.startX, touch.startY);

				const std::string patternString = "/block/lightpad/" + blockIndex + "/off";
				OSCAddressPattern pattern = OSCAddressPattern(patternString);
				if (! sender.send (pattern, touchIndex, x/2, y/2))
					showConnectionErrorMessage ("Error: could not send OSC message.");
				//bitmapProgram->setLED(roundToInt((touch.startX/2) * 16), roundToInt((touch.startY/2) * 16), Colours::black);
            }
            else
            {
                if (touchMessageTimesInLastSecond.size() > maxNumTouchMessagesPerSecond / 3)
                    return;

                gridPrograms[blockIndexInt]->sendTouch(touch.x, touch.y, touch.z, layout.touchColour);
				// xID, yID, x, y, z

				const std::string patternString = "/block/lightpad/" + blockIndex + "/position";
				OSCAddressPattern pattern = OSCAddressPattern(patternString);
				if (! sender.send (pattern, touchIndex, x/2, y/2, z))
					showConnectionErrorMessage ("Error: could not send OSC message.");
            }

            touchMessageTimesInLastSecond.add (now);

        }
    }

    void showConnectionErrorMessage (const String& messageText)
    {
        AlertWindow::showMessageBoxAsync (
            AlertWindow::WarningIcon,
            "Connection error",
            messageText,
            "OK");
    }
    /** Overridden from ControlButton::Listener. Called when a button on the Lightpad is pressed */
    void buttonPressed (ControlButton&, Block::Timestamp) override {
		if (! sender.send ("/block/lightpad/0/button", 1))
			showConnectionErrorMessage ("Error: could not send OSC message.");
	}

    /** Overridden from ControlButton::Listener. Called when a button on the Lightpad is released */
    void buttonReleased (ControlButton&, Block::Timestamp) override
    {
		if (! sender.send ("/block/lightpad/0/button", 0))
			showConnectionErrorMessage ("Error: could not send OSC message.");
    }

    void timerCallback() override
    {
        // Clear all LEDs
        for (uint32 x = 0; x < 15; ++x)
            for (uint32 y = 0; y < 15; ++y)
                bitmapProgram->setLED (x, y, Colours::black);

        // Determine which array to use based on waveshapeMode
        int* waveshapeY = nullptr;
        switch (waveshapeMode)
        {
            case 0:
                waveshapeY = sineWaveY;
                break;
            case 1:
                waveshapeY = squareWaveY;
                break;
            case 2:
                waveshapeY = sawWaveY;
                break;
            case 3:
                waveshapeY = triangleWaveY;
                break;
            default:
                break;
        }

        // For each X co-ordinate
        for (uint32 x = 0; x < 15; ++x)
        {
            // Find the corresponding Y co-ordinate for the current waveshape
            int y = waveshapeY[x + yOffset];

            // Draw a vertical line if flag is set or draw an LED circle
            if (y == -1)
            {
                for (uint32 i = 0; i < 15; ++i)
                    drawLEDCircle (x, i);
            }
            else if (x % 2 == 0)
            {
                drawLEDCircle (x, static_cast<uint32> (y));
            }
        }

        // Increment the offset to draw a 'moving' waveshape
        if (++yOffset == 30)
            yOffset -= 30;
    }

    /** Clears the old touch times */
    void clearOldTouchTimes (const Time now)
    {
        for (int i = touchMessageTimesInLastSecond.size(); --i >= 0;)
            if (touchMessageTimesInLastSecond.getReference(i) < now - juce::RelativeTime::seconds (0.33))
                touchMessageTimesInLastSecond.remove (i);
    }

    /** Removes TouchSurface and ControlButton listeners and sets activeBlock to nullptr */
    void detachActiveBlock()
    {
        if (auto surface = activeBlock->getTouchSurface())
            surface->removeListener (this);

        for (auto button : activeBlock->getButtons())
            button->removeListener (this);

        activeBlock = nullptr;
    }

    /** Sets the LEDGrid Program for the selected mode */
    void setLEDProgram(int blockIndex, LEDGrid* grid)
    {
		// Stop the redraw timer
		stopTimer();

		// Create a new DrumPadGridProgram for the LEDGrid
		gridPrograms[blockIndex] = new DrumPadGridProgram(*grid);

		// Set the LEDGrid program
		grid->setProgram(gridPrograms[blockIndex]);
		//bitmapProgram = new BitmapLEDProgram (*grid);
		//grid->setProgram(bitmapProgram);

		//bitmapProgram->setLED(5, 5, Colours::green);
		// Setup the grid layout
		(gridPrograms[blockIndex])->setGridFills(layout.numColumns, layout.numRows, layout.gridFillArray);
    }

    /** Draws a 'circle' on the Lightpad around an origin co-ordinate */
    void drawLEDCircle (uint32 x0, uint32 y0)
    {
        bitmapProgram->setLED (x0, y0, waveshapeColour);

		const uint32 minLedIndex = 0;
		const uint32 maxLedIndex = 14;

		bitmapProgram->setLED (jmin (x0 + 1, maxLedIndex), y0, waveshapeColour.withBrightness(0.4f));
		bitmapProgram->setLED (jmax (x0 - 1, minLedIndex), y0, waveshapeColour.withBrightness(0.4f));
		bitmapProgram->setLED (x0, jmin (y0 + 1, maxLedIndex), waveshapeColour.withBrightness(0.4f));
		bitmapProgram->setLED (x0, jmax (y0 - 1, minLedIndex), waveshapeColour.withBrightness(0.4f));

		bitmapProgram->setLED (jmin (x0 + 1, maxLedIndex), jmin (y0 + 1, maxLedIndex), waveshapeColour.withBrightness(0.1f));
		bitmapProgram->setLED (jmin (x0 + 1, maxLedIndex), jmax (y0 - 1, minLedIndex), waveshapeColour.withBrightness(0.1f));
		bitmapProgram->setLED (jmax (x0 - 1, minLedIndex), jmin (y0 + 1, maxLedIndex), waveshapeColour.withBrightness(0.1f));
		bitmapProgram->setLED (jmax (x0 - 1, minLedIndex), jmax (y0 - 1, minLedIndex), waveshapeColour.withBrightness(0.1f));
    }

    /**
     enum for the two modes
     */
    enum BlocksSynthMode
    {
        waveformSelectionMode = 0,
        playMode
    };
    BlocksSynthMode currentMode = playMode;

    //==============================================================================

    DrumPadGridProgram* gridPrograms[2];
    BitmapLEDProgram* bitmapProgram;

    SynthGrid layout;
    PhysicalTopologySource topologySource;
    Block::Ptr activeBlock;

    Array<juce::Time> touchMessageTimesInLastSecond;

    Colour waveshapeColour = Colours::red;

    int sineWaveY[45];
    int squareWaveY[45];
    int sawWaveY[45];
    int triangleWaveY[45];

    int waveshapeMode = 0;
    uint32 yOffset = 0;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
