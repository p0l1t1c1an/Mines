use std::ops::RangeInclusive;

use grid::{Grid, GridRowIter};
use itertools::iproduct;
use rand::prelude::*;

#[derive(Clone, Copy, Default, Debug)]
pub enum Tile {
    #[default]
    Uncleared,
    Cleared,
    Flagged,
    UnclearedMine,
    ClearedMine,
    FlaggedMine,
}

#[derive(Debug)]
pub struct Board {
    grid: Grid<Tile>,
    mines_count: usize,
    cleared_count: usize,
    flag_count: usize,
}

impl Default for Board {
    fn default() -> Self {
        Self::new(10, 10, 10)
    }
}

impl Board {
    pub fn new(r: usize, c: usize, n: usize) -> Self {
        let mut grid: Grid<Tile> = Grid::new(r, c);
        let mut rng = rand::thread_rng();
        let mut remaining_mines = n;
        let mut remaining_tiles = r * c;

        grid.iter_mut().for_each(|t| {
            let val: usize = rng.gen_range(0..remaining_tiles);
            if val < remaining_mines {
                *t = Tile::UnclearedMine;
                remaining_mines -= 1;
            }
            remaining_tiles -= 1;
        });

        Self {
            grid: grid,
            mines_count: n,
            cleared_count: 0,
            flag_count: 0,
        }
    }

    pub fn mines_count(&self) -> usize {
        self.mines_count
    }

    pub fn cleared_count(&self) -> usize {
        self.cleared_count
    }

    pub fn flag_count(&self) -> usize {
        self.flag_count
    }

    pub fn size(&self) -> usize {
        let (r, c) = self.grid.size();
        r * c
    }

    pub fn dimensions(&self) -> (usize, usize) {
        self.grid.size()
    }

    pub fn get_tile(&self, r: usize, c: usize) -> Tile {
        self.grid[(r, c)]
    }

    pub fn rows(&self) -> GridRowIter<Tile> {
        self.grid.iter_rows()
    }

    pub fn toggle_flag(&mut self, r: usize, c: usize) {
        self.grid[(r, c)] = match self.grid[(r, c)] {
            Tile::Uncleared => {
                self.flag_count += 1;
                Tile::Flagged
            }
            Tile::UnclearedMine => {
                self.flag_count += 1;
                Tile::FlaggedMine
            }
            Tile::Flagged => {
                self.flag_count -= 1;
                Tile::Uncleared
            }
            Tile::FlaggedMine => {
                self.flag_count -= 1;
                Tile::UnclearedMine
            }
            Tile::Cleared => Tile::Cleared,
            Tile::ClearedMine => Tile::ClearedMine,
        };
    }

    fn clear_tile(&mut self, r: usize, c: usize) {
        self.grid[(r, c)] = match self.grid[(r, c)] {
            Tile::Uncleared | Tile::Flagged | Tile::Cleared => Tile::Cleared,
            _ => Tile::ClearedMine,
        };
        self.cleared_count += 1;
    }

    /// Clear a tile and all adjacent tiles if it has zero adjacent mines
    /// Continues recursively ie if another 0 is hit
    pub fn clear_tile_with_zero(&mut self, r: usize, c: usize) {
        match self.get_tile(r, c) {
            Tile::Uncleared => {
                self.clear_tile(r, c);
                let mines = self.adjacent_mines(r, c);
                if mines == 0 {
                    let (r_range, c_range) = self.generate_ranges(r, c);
                    iproduct!(r_range, c_range).for_each(move |(row, col)| {
                        if let Some(g) = self.grid.get(row, col) {
                            match &g {
                                Tile::Cleared | Tile::ClearedMine => {}
                                _ => self.clear_tile_with_zero(row, col),
                            }
                        }
                    });
                }
            }
            Tile::UnclearedMine => self.clear_tile(r, c),
            _ => {}
        }
    }

    fn generate_ranges(
        &self,
        r: usize,
        c: usize,
    ) -> (RangeInclusive<usize>, RangeInclusive<usize>) {
        (
            if r == 0 { r..=r + 1 } else { r - 1..=r + 1 },
            if c == 0 { c..=c + 1 } else { c - 1..=c + 1 },
        )
    }

    pub fn adjacent_tiles<'a>(&'a self, r: usize, c: usize) -> impl Iterator<Item = &'a Tile> {
        let (r_range, c_range) = self.generate_ranges(r, c);
        iproduct!(r_range, c_range).filter_map(move |(row, col)| {
            if row == r && col == c {
                None
            } else {
                self.grid.get(row, col)
            }
        })
    }

    pub fn adjacent_flags(&self, r: usize, c: usize) -> usize {
        self.adjacent_tiles(r, c)
            .filter(|t| match t {
                Tile::Flagged | Tile::FlaggedMine => true,
                _ => false,
            })
            .count()
    }

    pub fn adjacent_mines(&self, r: usize, c: usize) -> usize {
        self.adjacent_tiles(r, c)
            .filter(|t| match t {
                Tile::UnclearedMine | Tile::FlaggedMine | Tile::ClearedMine => true,
                _ => false,
            })
            .count()
    }

    /// Clears adjacent tiles to selected tile when adjacent flags match adjacent mines
    /// If one of the adjacents in zero calls clear with zero function to remove everything
    /// adjacent to it as well
    pub fn clear_adj_tiles(&mut self, r: usize, c: usize) {
        match self.grid[(r, c)] {
            Tile::Cleared => {}
            _ => return,
        }

        let mines = self.adjacent_mines(r, c);
        let flags = self.adjacent_flags(r, c);

        if mines != flags {
            return;
        }

        let (r_range, c_range) = self.generate_ranges(r, c);
        iproduct!(r_range, c_range).for_each(move |(row, col)| {
            if let Some(g) = self.grid.get(row, col) {
                match &g {
                    Tile::Uncleared | Tile::UnclearedMine => self.clear_tile_with_zero(row, col),
                    _ => {}
                }
            }
        });
    }

    //    pub fn print(&self) {
    //        self.grid.iter_rows().for_each(|r| {
    //            r.for_each(|t| {
    //                if t.is_mine() {
    //                    print!("x ");
    //                } else {
    //                    print!("* ");
    //                }
    //            });
    //            println!();
    //        });
    //    }
    //
    //    pub fn print_adj_mines(&self) {
    //        for r in 0..self.grid.cols() {
    //            for c in 0..self.grid.rows() {
    //                if self.grid[(r, c)].is_mine() {
    //                    print!("x ");
    //                } else {
    //                    let num = self.adjacent_tiles(r, c).filter(|t| t.is_mine()).count();
    //                    print!("{num} ");
    //                }
    //            }
    //            println!();
    //        }
    //    }
}
