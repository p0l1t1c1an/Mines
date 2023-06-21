mod board;
mod style;

use board::{Board, Tile};
use style::ColorScheme;

use iced::alignment::{Horizontal, Vertical};
use iced::theme::Theme;
use iced::widget::{
    button, column, container, mouse_area, row, slider, text, Button, Column, Row, pick_list,
};
use iced::{executor, theme, time, Length};
use iced::{Alignment, Application, Command, Element, Settings, Subscription};

use iced_aw::floating_element::{Anchor, FloatingElement, Offset};

use std::time::Duration;

const NUMBERS: &'static [&'static str] = &[" ", "1", "2", "3", "4", "5", "6", "7", "8"];

pub fn main() -> iced::Result {
    Game::run(iced::Settings {
        window: iced::window::Settings {
            // Sizes based off of 25 px starting size
            size: (250, 280),
            min_size: Some((125, 155)),
            ..iced::window::Settings::default()
        },
        default_font: Some(include_bytes!(
            "/home/p0l1t1c1an/.fonts/Hack/HackNerdFont-Bold.ttf"
        )),
        default_text_size: 12.0,
        ..Settings::default()
    })
}
#[derive(Clone, Copy, Default)]
struct VisualElements {
    button_size: usize,
    slider_width: usize,
    slider_height: usize,
    slider_mines: usize,
    colorscheme: ColorScheme,
}

struct Game {
    board: Board,
    state: GameState,
    render: RenderState,
    seconds: usize,
    temp: VisualElements,
    real: VisualElements,
}

#[derive(Debug, Clone, Copy)]
enum Message {
    ResetClick,
    LeftClick(usize, usize),
    MiddleClick(usize, usize),
    RightClick(usize, usize),
    UpdateButtonSize(usize),
    UpdateSliderWidth(usize),
    UpdateSliderHeight(usize),
    UpdateSliderMines(usize),
    UpdateColorScheme(ColorScheme),
    Tick,
    Menu,
}

#[derive(Debug, Clone, Copy)]
enum GameState {
    Playing,
    Lost,
    Won,
}

#[derive(Debug, Clone, Copy)]
enum RenderState {
    Board,
    Menu,
}

impl Game {
    fn reset(&mut self) {
        self.real = self.temp;
        self.board = Board::new(self.real.slider_height, self.real.slider_width, self.real.slider_mines);
        self.state = GameState::Playing;
        self.render = RenderState::Board;
        self.seconds = 0;
    }

    fn left_click(&mut self, r: usize, c: usize) {
        self.board.clear_tile_with_zero(r, c);
        if self.board.cleared_count() + self.board.mines_count() >= self.board.size() {
            self.state = GameState::Won;
        }

        if let Tile::ClearedMine = self.board.get_tile(r, c) {
            self.state = GameState::Lost;
        }
    }

    fn middle_click(&mut self, r: usize, c: usize) {
        self.board.clear_adj_tiles(r, c);
        if self.board.cleared_count() >= self.board.size() - self.board.mines_count() {
            self.state = GameState::Won;
        }
        self.board.adjacent_tiles(r, c).for_each(|&t| {
            if let Tile::ClearedMine = t {
                self.state = GameState::Lost;
            }
        });
    }

    fn right_click(&mut self, r: usize, c: usize) {
        match self.board.get_tile(r, c) {
            Tile::Flagged | Tile::FlaggedMine => self.board.toggle_flag(r, c),
            Tile::Uncleared | Tile::UnclearedMine => {
                if self.board.flag_count() < self.board.mines_count() {
                    self.board.toggle_flag(r, c);
                }
            }
            _ => {}
        }
    }

    fn toggle_menu(&mut self) {
        self.render = match self.render {
            RenderState::Board => RenderState::Menu,
            RenderState::Menu => RenderState::Board,
        }
    }
}

impl Application for Game {
    type Message = Message;
    type Theme = Theme;
    type Executor = executor::Default;
    type Flags = ();

    fn new(_flags: ()) -> (Self, Command<Message>) {
        let visual = VisualElements {
            button_size: 25,
            slider_height: 10,
            slider_width: 10,
            slider_mines: 10,
            ..VisualElements::default()
        };
        (
            Self {
                board: Board::new(10, 10, 10),
                state: GameState::Playing,
                render: RenderState::Board,
                seconds: 0,
                temp: visual,
                real: visual
            },
            Command::none(),
        )
    }

    fn title(&self) -> String {
        String::from("Minesweeper")
    }

    fn update(&mut self, message: Message) -> Command<Message> {
        match self.state {
            GameState::Playing => {
                match message {
                    Message::ResetClick => self.reset(),
                    Message::LeftClick(r, c) => self.left_click(r, c),
                    Message::RightClick(r, c) => self.right_click(r, c),
                    Message::MiddleClick(r, c) => self.middle_click(r, c),
                    Message::Tick => self.seconds += 1,
                    Message::UpdateButtonSize(b) => self.temp.button_size = b, 
                    Message::UpdateSliderHeight(h) => self.temp.slider_height = h,
                    Message::UpdateSliderWidth(w) => self.temp.slider_width = w,
                    Message::UpdateSliderMines(n) => self.temp.slider_mines = n,
                    Message::UpdateColorScheme(c) => self.temp.colorscheme = c, 
                    Message::Menu => self.toggle_menu(),
                };
            }
            _ => match message {
                Message::ResetClick => self.reset(),
                Message::UpdateButtonSize(b) => self.temp.button_size = b, 
                Message::UpdateSliderHeight(h) => self.temp.slider_height = h,
                Message::UpdateSliderWidth(w) => self.temp.slider_width = w,
                Message::UpdateSliderMines(n) => self.temp.slider_mines = n,
                Message::UpdateColorScheme(c) => self.temp.colorscheme = c, 
                Message::Menu => self.toggle_menu(),
                _ => {}
            },
        }

        if let Message::ResetClick = message {
            iced::window::resize(
                (self.real.slider_width * self.real.button_size) as u32 + 2,
                30 + (self.real.slider_height * self.real.button_size) as u32,
            )
        } else {
            Command::none()
        }
    }

    fn subscription(&self) -> Subscription<Message> {
        match self.render {
            RenderState::Board => match self.state {
                GameState::Playing => time::every(Duration::from_secs(1)).map(|_| Message::Tick),
                _ => Subscription::none(),
            },
            _ => Subscription::none(),
        }
    }

    fn view(&self) -> Element<Message> {
        let mut rows: Vec<_> = match self.render {
            RenderState::Board => self
                .board
                .rows()
                .enumerate()
                .map(|(r, row)| {
                    let tiles = row
                        .enumerate()
                        .map(|(c, _col)| {
                            let button = button(
                                container(
                                    text(match self.board.get_tile(r, c) {
                                        Tile::Uncleared | Tile::UnclearedMine => " ",
                                        Tile::Cleared => NUMBERS[self.board.adjacent_mines(r, c)],
                                        Tile::ClearedMine => "󰷚",
                                        Tile::Flagged | Tile::FlaggedMine => "󰈻",
                                    })
                                    .width(Length::Fixed(9.0)),
                                )
                                .center_x()
                                .center_y()
                                .width(Length::Fill)
                                .height(Length::Fill),
                            )
                            .width(self.real.button_size as u16)
                            .height(self.real.button_size as u16)
                            .style(if (r + c) % 2 == 0 {
                                theme::Button::Custom(Box::new(self.real.colorscheme.light()))
                            } else {
                                theme::Button::Custom(Box::new(self.real.colorscheme.dark()))
                            });

                            mouse_area(match self.board.get_tile(r, c) {
                                Tile::Cleared | Tile::ClearedMine => button,
                                _ => button.on_press(Message::LeftClick(r, c)),
                            })
                            .on_right_press(Message::RightClick(r, c))
                            .on_middle_press(Message::MiddleClick(r, c))
                            .into()
                        })
                        .collect();

                    Row::with_children(tiles).into()
                })
                .collect(),
            RenderState::Menu => {
                let width_text = row!(
                    text("Width")
                        .width(Length::Fill)
                        .horizontal_alignment(Horizontal::Left),
                    text(format!("{:0>2}", self.temp.slider_width))
                        .width(Length::Fill)
                        .horizontal_alignment(Horizontal::Right),
                )
                .padding(10);
                let width_slider = slider(5..=50, self.temp.slider_width as u32, |t| {
                    Message::UpdateSliderWidth(t as usize)
                });

                let height_text = row!(
                    text("Height")
                        .width(Length::Fill)
                        .horizontal_alignment(Horizontal::Left),
                    text(format!("{:0>2}", self.temp.slider_height))
                        .width(Length::Fill)
                        .horizontal_alignment(Horizontal::Right),
                )
                .padding(10);
                let height_slider = slider(5..=50, self.temp.slider_height as u32, |t| {
                    Message::UpdateSliderHeight(t as usize)
                });

                let mines_text = row!(
                    text("Mine Count")
                        .width(Length::Fill)
                        .horizontal_alignment(Horizontal::Left),
                    text(format!("{:0>3}", self.temp.slider_mines))
                        .width(Length::Fill)
                        .horizontal_alignment(Horizontal::Right),
                )
                .padding(10);
                let mines_slider = slider(0..=500, self.temp.slider_mines as u32, |t| {
                    Message::UpdateSliderMines(t as usize)
                });

                let button_size = column!(text("Button Size"),
                    pick_list(vec!(25, 30, 35, 40), 
                    Some(self.temp.button_size), 
                    |size| {Message::UpdateButtonSize(size)}
                ));
 
                let colorscheme = column!(text("Color Scheme"), 
                    pick_list(ColorScheme::all_options(),
                    Some(self.temp.colorscheme),
                    |c| {Message::UpdateColorScheme(c)}
                ));

                let pick_lists = row!(button_size, colorscheme)
                    .spacing(150)
                    .padding(10);

                let reset = button("Reset")
                    .on_press(Message::ResetClick)
                    .style(theme::Button::Custom(Box::new(self.real.colorscheme.light())));

                let row = column!(
                    width_text,
                    width_slider,
                    height_text,
                    height_slider,
                    mines_text,
                    mines_slider,
                    pick_lists,
                    reset
                )
                .align_items(Alignment::Center);

                vec![row.into()]
            }
        };

        let duration = text(format!(" {:0>4}", self.seconds));
        let count = text(format!(
            "󰈻 {:0>3}",
            self.board.mines_count() - self.board.flag_count()
        ));

        let menu = Button::new(
            container("Menu")
                .center_x()
                .center_y()
                .width(Length::Fill)
                .height(Length::Fill),
        )
        .on_press(Message::Menu)
        .style(theme::Button::Custom(Box::new(self.real.colorscheme.light())));

        let top_row = row!(menu, duration, count)
            .spacing(40)
            .height(Length::Fixed(30.0))
            .align_items(Alignment::Center);

        rows.insert(0, top_row.into());

        let content = container(Column::with_children(rows).align_items(Alignment::Center))
            .style(self.real.colorscheme.background())
            .width(Length::Fill)
            .height(Length::Fill)
            .center_x()
            .align_y(Vertical::Top);

        if let RenderState::Board = self.render {
            match self.state {
                GameState::Playing => content.into(),
                _ => FloatingElement::new(content, || {
                    container(
                        column!(
                            text(if let GameState::Won = self.state {
                                "You Won!"
                            } else {
                                "You Lost"
                            }),
                            button("Reset")
                                .on_press(Message::ResetClick)
                                .style(theme::Button::Custom(Box::new(self.real.colorscheme.light()))),
                        )
                        .max_width(125)
                        .spacing(20)
                        .padding(20)
                        .align_items(Alignment::Center),
                    )
                    .style(self.real.colorscheme.background())
                    .into()
                })
                .anchor(Anchor::North)
                .offset(Offset { x: 0.0, y: 45.0 })
                .into(),
            }
        } else {
            content.into()
        }
    }
}
