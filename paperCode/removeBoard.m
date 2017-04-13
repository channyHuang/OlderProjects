function removeBoard
    imageDir = 'E:/RealFootData/data_3/pic/';
    imgfiles = dir([imageDir '/*.jpg']);
    for i = 1:length(imgfiles)
        imgfile = [imageDir imgfiles(i).name];
        I = imread(imgfile);
        %imshow(I);
        %title('original image');
        
        [imgPoints, boardSize] = detectCheckerboardPoints(I);
        
        if 0
        figure;
        imshow(I, 'InitialMagnification', 50);
        hold on;
        plot(imgPoints(:, 1), imgPoints(:, 2), '*-g');
        end
        
        Inew = I;
        for pt = 1:size(imgPoints, 1)
            xx = floor(imgPoints(pt, 1));
            yy = floor(imgPoints(pt, 2));
            for x = xx - 40 : xx + 40
                for y = yy - 40 : yy + 40
                    Inew(y, x, :) = 255;
                end
            end
        end
        
        %imshow(Inew);
        
        newDir = 'E:/RealFootData/data_3/maskPic/';
        savefilename = [newDir num2str(i - 1, '%08d') '.jpg'];
        imwrite(Inew, savefilename);
    end
end