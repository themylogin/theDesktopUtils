/* 
 * File:   main.cpp
 * Author: themylogin
 *
 * Created on 30 Июль 2012 г., 15:17
 */

#include <cmath>
#include <cstdio>
#include <ctime>

#include <assert.h>
#include <unistd.h>

#include <mpd/client.h>
#include <mpd/status.h>
#include <mpd/entity.h>
#include <mpd/tag.h>

/*
 * 
 */
int main(int argc, char** argv)
{
    int music_will_finish_at;
    int last_printed_music_will_finish_at = -2;
    FILE* fp = fopen("/tmp/musicjokes.txt", "w");
    
    // Цикл переподключения к MPD-серверу
    while (1)
    {
        // Подключение к MPD-серверу
        struct mpd_connection* conn = mpd_connection_new("192.168.0.4", 6600, 30000);
        if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS)
        {
            goto finish;
        }

        // Цикл опроса MPD-сервера
        while (1)
        {
            music_will_finish_at = -1;

            // Получаем информацию о состоянии MPD-сервера
            mpd_send_status(conn);
            struct mpd_status* status;
            if ((status = mpd_recv_status(conn)) == NULL)
            {
                goto finish;
            }
            if (mpd_status_get_state(status) == MPD_STATE_PLAY)
            {
                if (mpd_status_get_repeat(status))
                {
                    music_will_finish_at = 0;
                }
                else
                {
                    // Что-то воспроизводим, узнаем что
                    mpd_send_list_queue_meta(conn);

                    struct mpd_entity* entity;
                    music_will_finish_at = time(NULL);
                    for (int i = 0; (entity = mpd_recv_entity(conn)) != NULL; i++)
                    {
                        assert(mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_SONG);

                        if (i > mpd_status_get_song_pos(status))
                        {
                            // Песня будет воспроизведена в будущем
                            music_will_finish_at += mpd_song_get_duration(mpd_entity_get_song(entity));
                        }
                        else if (i == mpd_status_get_song_pos(status))
                        {
                            // Песня воспроизводится сейчас
                            music_will_finish_at += mpd_status_get_total_time(status) - mpd_status_get_elapsed_time(status);
                        }

                        mpd_entity_free(entity);
                    }
                }
            }
            else
            {
                // Ничего не воспроизводим
                music_will_finish_at = -1;
            }

            if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS)
            {
                goto finish;
            }
            
            if (music_will_finish_at != last_printed_music_will_finish_at)
            {
                if (music_will_finish_at < 0)
                {
                    fprintf(fp, "Без музыки скучно, включи музыку!\n");
                    fflush(fp);
                }
                else if (music_will_finish_at == 0)
                {
                    fprintf(fp, "Музыка не закончится никогда!\n");
                    fflush(fp);
                }
                else
                {
                    if (std::abs(last_printed_music_will_finish_at - music_will_finish_at) > 1)
                    {                    
                        char buf[6];
                        struct tm* timeinfo = localtime((time_t*)&music_will_finish_at);
                        strftime(buf, 6, "%H:%M", timeinfo);

                        fprintf(fp, "Музыка закончится в %s\n", buf);
                        fflush(fp);
                    }
                }
                
                last_printed_music_will_finish_at = music_will_finish_at;
            }
            
            sleep(1);
        }

        // Сюда переходим в случае ошибки: отключаемся и потом подключимся снова
        finish:
        mpd_connection_free(conn);
    }
    
    return 0;
}
