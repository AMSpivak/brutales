#ifndef GL_DUNGEON
#define GL_DUNGEON
#include <vector>
class GlDungeon
{
public:

    GlDungeon(size_t x, size_t y, size_t z)
    {
        m_dungeon_width = x;
        m_dungeon_height = y;
        m_dungeon_floors = z;
        m_dungeon_map_objects.resize(m_dungeon_width*m_dungeon_height*m_dungeon_floors,0);
        //m_dungeon_map_objects[1] = 1;
        m_dungeon_map_tiles.resize(m_dungeon_width*m_dungeon_height*m_dungeon_floors,0);
        /*for(size_t p = 0; p < m_dungeon_width; p++)
        {
            m_dungeon_map_objects[0*m_dungeon_width*m_dungeon_height + m_dungeon_width*9 +p] = 2;
            m_dungeon_map_objects[0*m_dungeon_width*m_dungeon_height + m_dungeon_width*p +0] = 4;
            m_dungeon_map_objects[0*m_dungeon_width*m_dungeon_height + m_dungeon_width*p +3] = 4;
        }
        m_dungeon_map_objects[0*m_dungeon_width*m_dungeon_height + m_dungeon_width*9 +9] = 3;
        m_dungeon_map_objects[0*m_dungeon_width*m_dungeon_height + m_dungeon_width*9 +3] = 3;   
        m_dungeon_map_objects[0*m_dungeon_width*m_dungeon_height + m_dungeon_width*9 ] = 3;
    
        m_dungeon_map_objects[0*m_dungeon_width*m_dungeon_height + m_dungeon_width*4 +3] = 0;
        m_dungeon_map_objects[0*m_dungeon_width*m_dungeon_height + m_dungeon_width*5 +3] = 0;
        m_dungeon_map_objects[0*m_dungeon_width*m_dungeon_height + m_dungeon_width*3 +4] = 1;
        m_dungeon_map_objects[0*m_dungeon_width*m_dungeon_height + m_dungeon_width*6 +4] = 1;*/

    } 
    int GetMapTilesIndex(int x, int y, int z)
    {
        if(
            (x>=m_dungeon_width)
            || (y>=m_dungeon_height)
            || (z>=m_dungeon_floors)
            || (x<0)
            || (y<0)
            || (z<0)
        )
        {
            return -1;
        }
        return m_dungeon_map_tiles[z*m_dungeon_width*m_dungeon_height + m_dungeon_width*y +x];
    }
    int GetMapObjectIndex(int x, int y, int z)
    {
        if(
            (x<m_dungeon_width)
            || (y<m_dungeon_height)
            || (z<m_dungeon_floors)
            || (x>=0)
            || (y>=0)
            || (z>=0)
        )
        {
            return m_dungeon_map_objects[z * m_dungeon_width * m_dungeon_height + m_dungeon_width * y + x];
        }

        return -1;
    }
    inline size_t Width()
    {
        return m_dungeon_width;
    }
    inline size_t Height()
    {
        return m_dungeon_height;
    }
    inline size_t Floors()
    {
        return m_dungeon_floors;
    }

    void SetTile(int x, int y, int z,size_t value)
    {
        m_dungeon_map_tiles[z*m_dungeon_width*m_dungeon_height + m_dungeon_width*y +x] =  value;        
    }

    void SetObject(int x, int y, int z,size_t value)
    {
        m_dungeon_map_objects[z*m_dungeon_width*m_dungeon_height + m_dungeon_width*y +x] =  value;
    }
private:
    std::vector <int> m_dungeon_map_tiles;
    std::vector <int> m_dungeon_map_objects;
    size_t m_dungeon_floors;
    size_t m_dungeon_width;
    size_t m_dungeon_height;
};

#endif