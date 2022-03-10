use std::f64;
use std::vec;
use std::fs::File;
use std::io::Write;

struct Dvec3 { x: f64, y: f64, z: f64 }
impl Dvec3 {
    fn new(vx: f64, vy: f64, vz: f64) -> Dvec3 { Dvec3 { x: vx, y: vy, z: vz } }
}

fn compute_image(cx:f64, cy:f64, sx:f64, sy:f64) -> Dvec3
{
    let zoom = 0.93;
    let mns = zoom / f64::min(sx, sy);
    let mut zx = (cx * 2.0 - sx) * mns;
    let mut zy = (cy * 2.0 - sy) * mns;
    let px = 1.0;
    let py = 0.3;

    let mut d = 0.0;
    let mut tmp_x;
    for _n in 1..12
    {
        d = zx * zx + zy * zy;
        if d < 4.0
        {
            tmp_x = zx;
            zx = zx * zx - zy * zy - px;
            zy = 2.0 * tmp_x * zy - py;
        }
    }

    let de = f64::sqrt(d) * f64::log(d, 10.0);
    let dd = 1.0 / d;

    return Dvec3::new(
        f64::clamp(0.15 * dd + de, 0.0, 1.0),
        f64::clamp(0.1 * dd + de, 0.0, 1.0),
        f64::clamp(0.1 * dd + de, 0.0, 1.0),
    );
}

fn generate_shader(w:usize, h:usize) -> Vec<u8>
{
    let datasize = 3 * w * h;
    let mut final_img:Vec<u8> = vec![0; datasize];
    for x in 0..w {
        for j in 0..h {
            let y = (h-1)-j;
            let pixel_color = compute_image(x as f64, y as f64, w as f64, h as f64);
            final_img[(x+y*w)*3+2] = (pixel_color.x * 255.0) as u8;
            final_img[(x+y*w)*3+1] = (pixel_color.y * 255.0) as u8;
            final_img[(x+y*w)*3+0] = (pixel_color.z * 255.0) as u8;
        }
    }

    final_img
}

fn generate_bitmap(w:usize, h:usize, img:Vec<u8>) -> std::io::Result<()>
{
    let headersize = 54;
    let filesize = headersize as usize + img.len();

    let mut bmp_file_header:Vec<u8> = vec![0; 14];
    bmp_file_header[0] = 'B' as u8;
    bmp_file_header[1] = 'M' as u8;
    bmp_file_header[2] = (filesize) as u8;
    bmp_file_header[3] = (filesize >> 08) as u8;
    bmp_file_header[4] = (filesize >> 16) as u8;
    bmp_file_header[5] = (filesize >> 24) as u8;
    bmp_file_header[10] = headersize;

    let mut bmp_info_header:Vec<u8> = vec![0; 40];
    bmp_info_header[0] = 40;
    bmp_info_header[4] = (w) as u8;
    bmp_info_header[5] = (w>>8) as u8;
    bmp_info_header[6] = (w>>16) as u8;
    bmp_info_header[7] = (w>>24) as u8;
    bmp_info_header[8] = (h) as u8;
    bmp_info_header[9] = (h>>8) as u8;
    bmp_info_header[10] = (h>>16) as u8;
    bmp_info_header[11] = (h>>24) as u8;
    bmp_info_header[12] = 1;
    bmp_info_header[14] = 24;

    let bmp_pad:Vec<u8> = vec![0; 3];

    let mut file = File::create("julia.bmp")?;
    file.write_all (&bmp_file_header)?;
    file.write_all(&bmp_info_header)?;

    for i in 0..h {
        let pos = w*(h-i-1)*3;
        file.write(&img[pos..(pos+3*w)])?;
        file.write(&bmp_pad[..((4 - (w * 3) % 4) % 4)])?;
    }
    file.sync_all()?;
    Ok(())
}

fn main() -> std::io::Result<()>
{
    let w:usize = 2560;
	let h:usize = 1440;

	generate_bitmap(w,h, generate_shader(w,h))?;

    Ok(())
}
