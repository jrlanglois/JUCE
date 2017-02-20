/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2015 - ROLI Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

static Colour createColourFromAE (const aefilter::AEColor& c)
{
    return Colour::fromFloatRGBA ((float) c.red(), (float) c.green(), (float) c.blue(), (float) c.alpha());
}

class AfterEffectsComponent : public Component
{
public:
    AfterEffectsComponent()
    {
    }
    
    bool parse (const File& file)
    {
        MemoryBlock data;
        if (! file.loadFileAsData (data))
            return false;

        layout = new aefilter::AELayout();
        if (! layout->ParseFromArray (data.getData(), (int) data.getSize()))
        {
            layout = nullptr;
            return false;
        }

        setName (getActiveComp().name());
        setSize (getActiveComp().width(), getActiveComp().height());

        parseLayout();
        parseComponents();
    
        return layout != nullptr;
    }

private:
    ScopedPointer<aefilter::AELayout> layout;

    OwnedArray<Component> comps;
    Array<Typeface::Ptr> typefaces;

    const aefilter::AEComposition& getActiveComp() { return layout->activecomp(); }
    
    void addChild (Component* component, const aefilter::AELayer& layer)
    {
        if (component != nullptr)
        {
            component->setName (layer.name());
            component->setComponentID (String (layer.id()));
            
            NamedValueSet& props = component->getProperties();
            for (int i = 0; i < layer.properties().size(); ++i)
            {
                const aefilter::AEKeyPair& property = layer.properties (i);

                if (property.has_key())
                    props.set (Identifier (property.key()), String (property.value()));
            }
            
            addChildComponent (comps.add (component));
        }
    }
    
    static Label* createLabel (const aefilter::AELayer& layer)
    {
        jassert (layer.has_textinfo());

        const aefilter::AETextInfo& textInfo = layer.textinfo();

        Label* label = new Label();

        Font font ((float) textInfo.fontsize());
        font.setTypefaceName (textInfo.fontname());
        label->setFont (font);
        
        label->setColour (Label::textColourId, createColourFromAE (textInfo.fontcolor()));
        label->setBorderSize (BorderSize<int>());
        
        if (textInfo.has_ellipsis() && textInfo.ellipsis())
        {
            label->setMinimumHorizontalScale (1.0f);
        }
        
        switch (textInfo.justification())
        {
            case aefilter::AE_TEXT_RIGHT:
                label->setJustificationType (Justification::centredRight);
            break;
            
            case aefilter::AE_TEXT_CENTER:
                label->setJustificationType (Justification::centred);
            break;
            
            default:
            break;
        }

        if (textInfo.has_text())
            label->setText (textInfo.text(), NotificationType::dontSendNotification);
        
        if (textInfo.has_isallcaps() && textInfo.isallcaps())
            label->setText (label->getText().toUpperCase(), NotificationType::dontSendNotification);
        
        return label;
    }

    void parseLayout()
    {
        for (int i = 0; i < layout->sources_size(); ++i)
        {
            const aefilter::AESource& source = layout->sources (i);
        
            switch (source.srctype())
            {
                //Add any specific fonts:
                case aefilter::AE_FONT_SRC:
                {
                    const File fontFile (source.name());
                    MemoryBlock fontData;
                    
                    if (fontFile.existsAsFile())
                        if (fontFile.loadFileAsData (fontData))
                            typefaces.add (Typeface::createSystemTypefaceFor (fontData.getData(), fontData.getSize()));
                }
                break;
                
                //Add any specific images:
                case aefilter::AE_IMAGE_SRC:
                {
                    const File imageFile (source.name());
                    
                    if (imageFile.existsAsFile())
                    {
                        const Image image (ImageFileFormat::loadFrom (imageFile));
                    
                        if (! image.isNull())
                        {
                            const int64 id = source.has_uid() ? (int64) source.uid()
                                                              : imageFile.hashCode64();
                            ImageCache::addImageToCache (image, id);
                        }
                    }
                }
                break;
            
                default:
                break;
            };
        }
    }

    void parseComponents()
    {
        for (int i = 0; i < getActiveComp().layer_size(); ++i)
        {
            const aefilter::AELayer& layer = getActiveComp().layer (i);
            
            if (layer.has_textinfo())
            {
                addChild (createLabel (layer), layer);
                return;
            }
            
            Component* component = nullptr;
            aefilter::AESourceType shit = aefilter::AE_NULL_SRC;
            switch (shit)//layer.srctype())
            {
                case aefilter::AE_NULL_SRC:
                {
                    component = new Component();
                }
                break;
                
                case aefilter::AE_IMAGE_SRC:
                {
                    component = new Component();
                }
                break;
    
                case aefilter::AE_IMAGE_NPATCH_SRC:
                {
                    component = new Component();
                }
                break;
                
                case aefilter::AE_SPRITE_SRC:
                {
                    component = new Component();
                }
                break;
                
                case aefilter::AE_SOLID_SRC:
                {
                    Path path;
                    path.addRectangle (0.0f, 0.0f, 0.0f, 0.0f);

                    DrawablePath* dp = new DrawablePath();
                    dp->setPath (path);
                    //dp->setFill (FillType (createColourFromAE (source.color())));

                    component = dp;
                }
                break;
                
                case aefilter::AE_COMP_SRC:
                {
                    component = new Component();
                }
                break;
                
                case aefilter::AE_MASK_SRC:
                {
                    component = new Component();
                }
                break;
                
                case aefilter::AE_ADJUSTMENT_SRC:
                {
                    component = new Component();
                }
                break;
                
                case aefilter::AE_FONT_SRC:
                {
                    Label* label = new Label();
            
                    component = label;
                }
                break;
            
                case aefilter::AE_SRC_NONE:
                default:
                break;
            };

            if (component != nullptr)
                addChild (component, layer);
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AfterEffectsComponent)
};

Component* AfterEffectsImporter::load (const File& file)
{
    ScopedPointer<AfterEffectsComponent> comp (new AfterEffectsComponent());
    if (comp->parse (file))
        return comp.release();

    return nullptr;
}
