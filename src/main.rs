mod board;
mod icons;
mod style;

use board::{Board, Tile};
use style::{ColorScheme, Size, TransparentStyle};

use iced::alignment::{Horizontal, Vertical};
use iced::theme::Theme;
use iced::widget::{
    button, column, container, mouse_area, pick_list, row, scrollable, slider, svg, text, Button,
    Column, Row,
};
use iced::{executor, time};
use iced::{Alignment, Application, Command, Element, Length, Settings, Subscription};

use iced_aw::floating_element::{Anchor, FloatingElement, Offset};

use std::time::Duration;

const NUMBERS: &'static [&'static str] = &[" ", "1", "2", "3", "4", "5", "6", "7", "8"];

pub fn main() -> iced::Result {
    Game::run(iced::Settings {
        window: iced::window::Settings {
            min_size: Some((450, 500)), // 10x10 on largest Size
            ..iced::window::Settings::default()
        },
        default_text_size: 15.0,
        ..Settings::default()
    })
}

#[derive(Clone, Copy, Debug)]
struct VisualElements {
    size: Size,
    slider_width: usize,
    slider_height: usize,
    slider_mines: usize,
    colorscheme: ColorScheme,
}

impl Default for VisualElements {
    fn default() -> Self {
        Self {
            size: Size::default(),
            slider_width: 10,
            slider_mines: 10,
            slider_height: 10,
            colorscheme: ColorScheme::default(),
        }
    }
}

#[derive(Debug, Clone, Copy, Default)]
enum GameState {
    #[default]
    Playing,
    Lost,
    Won,
}

#[derive(Debug, Clone, Copy, Default)]
enum RenderState {
    #[default]
    Board,
    Menu,
}

#[derive(Debug, Default)]
struct Game {
    board: Board,
    state: GameState,
    render: RenderState,
    seconds: usize,
    temp: VisualElements,
    real: VisualElements,
}

#[derive(Debug, Clone)]
enum Message {
    ResetClick,
    LeftClick(usize, usize),
    MiddleClick(usize, usize),
    RightClick(usize, usize),
    UpdateSize(Size),
    UpdateSliderWidth(usize),
    UpdateSliderHeight(usize),
    UpdateSliderMines(usize),
    UpdateColorScheme(ColorScheme),
    Tick,
    Menu,
}

impl Game {
    fn reset(&mut self) {
        self.real = self.temp;
        self.board = Board::new(
            self.real.slider_height,
            self.real.slider_width,
            self.real.slider_mines,
        );
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
        (Default::default(), Command::none())
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
                    Message::UpdateSize(s) => self.temp.size = s,
                    Message::UpdateSliderHeight(h) => self.temp.slider_height = h,
                    Message::UpdateSliderWidth(w) => self.temp.slider_width = w,
                    Message::UpdateSliderMines(n) => self.temp.slider_mines = n,
                    Message::UpdateColorScheme(c) => self.temp.colorscheme = c,
                    Message::Menu => self.toggle_menu(),
                };
            }
            _ => match message {
                Message::ResetClick => self.reset(),
                Message::UpdateSize(s) => self.temp.size = s,
                Message::UpdateSliderHeight(h) => self.temp.slider_height = h,
                Message::UpdateSliderWidth(w) => self.temp.slider_width = w,
                Message::UpdateSliderMines(n) => self.temp.slider_mines = n,
                Message::UpdateColorScheme(c) => self.temp.colorscheme = c,
                Message::Menu => self.toggle_menu(),
                _ => {}
            },
        }

        Command::none()
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
        let rows: Vec<_> = match self.render {
            RenderState::Board => self
                .board
                .rows()
                .enumerate()
                .map(|(r, row)| {
                    let tiles = row
                        .enumerate()
                        .map(|(c, _col)| {
                            let color = if (r + c) % 2 == 0 {
                                self.real.colorscheme.light()
                            } else {
                                self.real.colorscheme.dark()
                            };

                            let button = button(
                                match self.board.get_tile(r, c) {
                                    Tile::Uncleared | Tile::UnclearedMine => {
                                        container(text(" ").size(self.real.size.text()))
                                    }
                                    Tile::Cleared => container(
                                        text(NUMBERS[self.board.adjacent_mines(r, c)])
                                            .size(self.real.size.text()),
                                    ),
                                    Tile::ClearedMine => container(
                                        svg(svg::Handle::from_memory(icons::MINE))
                                            .style(color.reverse().into()),
                                    ),
                                    Tile::Flagged | Tile::FlaggedMine => container(
                                        svg(svg::Handle::from_memory(icons::FLAG))
                                            .style(color.clone().into()),
                                    ),
                                }
                                .center_x()
                                .center_y()
                                .width(Length::Fill)
                                .height(Length::Fill),
                            )
                            .width(self.real.size.mine())
                            .height(self.real.size.mine())
                            .style(color.into());

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
                        .horizontal_alignment(Horizontal::Left)
                        .size(self.real.size.text()),
                    text(format!("{:0>2}", self.temp.slider_width))
                        .width(Length::Fill)
                        .horizontal_alignment(Horizontal::Right)
                        .size(self.real.size.text()),
                )
                .padding(10);
                let width_slider = slider(5..=50, self.temp.slider_width as u32, |t| {
                    Message::UpdateSliderWidth(t as usize)
                })
                .style(self.real.colorscheme.light());

                let height_text = row!(
                    text("Height")
                        .width(Length::Fill)
                        .horizontal_alignment(Horizontal::Left)
                        .size(self.real.size.text()),
                    text(format!("{:0>2}", self.temp.slider_height))
                        .width(Length::Fill)
                        .horizontal_alignment(Horizontal::Right)
                        .size(self.real.size.text()),
                )
                .padding(10);
                let height_slider = slider(5..=50, self.temp.slider_height as u32, |t| {
                    Message::UpdateSliderHeight(t as usize)
                })
                .style(self.real.colorscheme.light());

                let mines_text = row!(
                    text("Mine Count")
                        .width(Length::Fill)
                        .horizontal_alignment(Horizontal::Left)
                        .size(self.real.size.text()),
                    text(format!("{:0>3}", self.temp.slider_mines))
                        .width(Length::Fill)
                        .horizontal_alignment(Horizontal::Right)
                        .size(self.real.size.text()),
                )
                .padding(10);
                let mines_slider = slider(0..=500, self.temp.slider_mines as u32, |t| {
                    Message::UpdateSliderMines(t as usize)
                })
                .style(self.real.colorscheme.light());

                let button_size = column!(
                    text("Display Size").size(self.real.size.text()),
                    pick_list(Size::all_options(), Some(self.temp.size), |size| {
                        Message::UpdateSize(size)
                    })
                    .style(self.real.colorscheme.light())
                    .text_size(self.real.size.text())
                )
                .spacing(5);

                let colorscheme = column!(
                    text("Color Scheme").size(self.real.size.text()),
                    pick_list(
                        ColorScheme::all_options(),
                        Some(self.temp.colorscheme),
                        |c| { Message::UpdateColorScheme(c) }
                    )
                    .style(self.real.colorscheme.light())
                    .text_size(self.real.size.text())
                )
                .spacing(5);

                let pick_lists = row!(button_size, colorscheme).spacing(150).padding(10);

                let reset = button(text("Reset").size(self.real.size.text()))
                    .on_press(Message::ResetClick)
                    .padding(7.5)
                    .style(self.real.colorscheme.light().into());

                let menu = iced::widget::column!(
                    width_text,
                    width_slider,
                    height_text,
                    height_slider,
                    mines_text,
                    mines_slider,
                    pick_lists,
                    reset
                )
                .align_items(Alignment::Center)
                .max_width(self.real.size.mine() * 20);

                vec![menu.into()]
            }
        };

        let duration = row!(
            svg(svg::Handle::from_memory(icons::STOPWATCH))
                .height(self.real.size.text())
                .width(self.real.size.text())
                .style(self.real.colorscheme.background().into()),
            text(format!("{:0>4}", self.seconds)).size(self.real.size.text())
        )
        .align_items(Alignment::Center)
        .spacing(5);

        let count = row!(
            svg(svg::Handle::from_memory(icons::FLAG))
                .height(self.real.size.text())
                .width(self.real.size.text())
                .style(self.real.colorscheme.background().into()),
            text(format!(
                "{:0>3}",
                self.board.mines_count() - self.board.flag_count()
            ))
            .size(self.real.size.text())
        )
        .height(self.real.size.mine())
        .spacing(5)
        .align_items(Alignment::Center);

        let menu = Button::new(
            container(text("Menu").size(self.real.size.text()))
                .center_x()
                .center_y()
                .width(Length::Fill)
                .height(Length::Fill),
        )
        .on_press(Message::Menu)
        .padding(7.5)
        .style(self.real.colorscheme.light().into());

        let top_row = row!(menu, duration, count)
            .spacing(30)
            .height(Length::Fixed(50.0))
            .align_items(Alignment::Center);

        let board = scrollable(Column::with_children(rows).align_items(Alignment::Center))
            .horizontal_scroll(scrollable::Properties::default())
            .style(TransparentStyle);

        let content = container(column!(top_row, board).align_items(Alignment::Center))
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
                            })
                            .size(self.real.size.text()),
                            button(text("Reset").size(self.real.size.text()))
                                .on_press(Message::ResetClick)
                                .padding(7.5)
                                .style(self.real.colorscheme.light().into()),
                        )
                        .max_width(225)
                        .spacing(20)
                        .padding(20)
                        .align_items(Alignment::Center),
                    )
                    .style(self.real.colorscheme.background())
                    .into()
                })
                .anchor(Anchor::North)
                .offset(Offset { x: 0.0, y: 60.0 })
                .into(),
            }
        } else {
            content.into()
        }
    }
}
