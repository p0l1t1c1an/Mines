
use iced::theme::{Container, Theme, Button, Slider};
use iced::widget::{button, container, slider};
use iced::widget::slider::{Handle, HandleShape, Rail};
use iced::color;
use iced::{Background, Color};

use std::fmt::{self, Display};

#[derive(Clone, Copy)]
pub struct SimpleStyle {
    back: Color,
    highlight_back: Color,
    text: Color,
}

impl iced::widget::button::StyleSheet for SimpleStyle {
    type Style = Theme;

    fn active(&self, _style: &Self::Style) -> button::Appearance {
        button::Appearance {
            background: Some(Background::Color(self.back)),
            text_color: self.text,
            ..button::Appearance::default()
        }
    }

    fn hovered(&self, _style: &Self::Style) -> button::Appearance {
        button::Appearance {
            background: Some(Background::Color(self.highlight_back)),
            text_color: self.text,
            ..button::Appearance::default()
        }
    }

    fn disabled(&self, _style: &Self::Style) -> button::Appearance {
        button::Appearance {
            background: Some(Background::Color(self.text)),
            text_color: self.back,
            ..button::Appearance::default()
        }
    }
}

impl From<SimpleStyle> for Button {
    fn from(style: SimpleStyle) -> Self {
        Self::Custom(Box::new(style))
    }
}

impl iced::widget::container::StyleSheet for SimpleStyle {
    type Style = Theme;

    fn appearance(&self, _style: &Self::Style) -> container::Appearance {
        container::Appearance {
            background: Some(Background::Color(self.back)),
            text_color: Some(self.text),
            border_color: self.highlight_back,
            border_width: 1.0,
            ..container::Appearance::default()
        }
    }
}

impl From<SimpleStyle> for Container {
    fn from(style: SimpleStyle) -> Self {
        Self::Custom(Box::new(style))
    }
}

impl iced::widget::slider::StyleSheet for SimpleStyle {
    type Style = Theme; 

    fn active(&self, _style: &Self::Style) -> slider::Appearance {
        slider::Appearance {
            rail: Rail {
                colors: (self.back, self.text),
                width: 5.0,
            },
            handle: Handle {
                shape: HandleShape::Circle{radius: 7.5},
                color: self.text,
                border_width: 1.0,
                border_color: self.highlight_back,
            }
        }
    }

    fn hovered(&self, _style: &Self::Style) -> slider::Appearance {
        // Averages color with white 
        let mut bright = self.text.into_rgba8(); 
        bright.iter_mut().for_each(|c| {
            *c = *c/2 + 128; 
        });

        slider::Appearance {
            rail: Rail {
                colors: (self.back, self.text),
                width: 5.0,
            },
            handle: Handle {
                shape: HandleShape::Circle{radius: 7.5},
                color: Color::from_rgb8(bright[0], bright[1], bright[2]),
                border_width: 1.0,
                border_color: self.highlight_back,
            }
        }
    }

    fn dragging(&self, _style: &Self::Style) -> slider::Appearance {
        self.hovered(_style)    
    }
}

impl From<SimpleStyle> for Slider {
    fn from(style: SimpleStyle) -> Self {
        Self::Custom(Box::new(style))
    }
}


#[derive(Clone, Copy, Debug, Default, Eq, PartialEq)]
pub enum ColorScheme {
    #[default]
    Purple,
    Red,
    Blue,
    Green,
    Yellow,
    Grey,
}

impl Display for ColorScheme {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", match self {
            Self::Purple => "Purple",
            Self::Red => "Red",
            Self::Blue => "Blue",
            Self::Green => "Green",
            Self::Yellow => "Yellow",
            Self::Grey => "Grey",
        })
    }
}

impl ColorScheme {
    pub fn all_options() -> Vec<Self> {
        vec!(
            Self::Purple,
            Self::Red,
            Self::Blue,
            Self::Green,
            Self::Yellow,
            Self::Grey,
        )
    }

    pub fn light(&self) -> SimpleStyle {
        match self {
            ColorScheme::Purple =>  SimpleStyle {
                back: color!(0x6E49AD),
                highlight_back: color!(0x8161BD),
                text: color!(0xC6B7E1),
            },
            ColorScheme::Red =>  SimpleStyle {
                back: color!(0xAD4859),
                highlight_back: color!(0xBD6170),
                text: color!(0xE1B7BE),
            },
            ColorScheme::Blue =>  SimpleStyle {
                back: color!(0x4848AD),
                highlight_back: color!(0x6161BD),
                text: color!(0xCCCCF5),
            },
            ColorScheme::Green =>  SimpleStyle {
                back: color!(0x49AD50),
                highlight_back: color!(0x61BD67),
                text: color!(0xB7E1BA),
            },
            ColorScheme::Yellow =>  SimpleStyle {
                back: color!(0xDFCF48),
                highlight_back: color!(0xE7DB74),
                text: color!(0xF3EDB9),
            },
            ColorScheme::Grey =>  SimpleStyle {
                back: color!(0xA9A9B1),
                highlight_back: color!(0xDFDFE2),
                text: color!(0xEAEAEC),
            },
        }
    }

    pub fn dark(&self) -> SimpleStyle {
        match self {
            ColorScheme::Purple =>  SimpleStyle {
                back: color!(0x5B3C90),
                highlight_back: color!(0x8161BD),
                text: color!(0xA48CCF),
            },
            ColorScheme::Red =>  SimpleStyle {
                back: color!(0x903C4A),
                highlight_back: color!(0xBD6170),
                text: color!(0xCF8C97),
            },
            ColorScheme::Blue =>  SimpleStyle {
                back: color!(0x3C3C90),
                highlight_back: color!(0x6161BD),
                text: color!(0x9999EA),
            },
            ColorScheme::Green =>  SimpleStyle {
                back: color!(0x3C9042),
                highlight_back: color!(0x61BD67),
                text: color!(0x8CCF90),
            },
            ColorScheme::Yellow =>  SimpleStyle {
                back: color!(0xD0BF25),
                highlight_back: color!(0xE7DB74),
                text: color!(0xEAE085),
            },
            ColorScheme::Grey =>  SimpleStyle {
                back: color!(0x898994),
                highlight_back: color!(0xDFDFE2),
                text: color!(0xC9C9CF),
            },
        }
    }

    pub fn background(&self) -> SimpleStyle {
        match self {
            ColorScheme::Purple =>  SimpleStyle {
                back: color!(0x3F2A65),
                highlight_back: color!(0x8161BD),
                text: color!(0xA48CCF),
            },
            ColorScheme::Red =>  SimpleStyle {
                back: color!(0x652A34),
                highlight_back: color!(0xBD6170),
                text: color!(0xCF8C97),
            },
            ColorScheme::Blue =>  SimpleStyle {
                back: color!(0x242456),
                highlight_back: color!(0x6161BD),
                text: color!(0x9999EA),
            },
            ColorScheme::Green =>  SimpleStyle {
                back: color!(0x2A652E),
                highlight_back: color!(0x61BD67),
                text: color!(0x8CCF90),
            },
            ColorScheme::Yellow =>  SimpleStyle {
                back: color!(0xAD9F1F),
                highlight_back: color!(0xE7DB74),
                text: color!(0xEAE085),
            },
            ColorScheme::Grey =>  SimpleStyle {
                back: color!(0x747481),
                highlight_back: color!(0xDFDFE2),
                text: color!(0xC9C9CF),
            },
        }
    }
}

